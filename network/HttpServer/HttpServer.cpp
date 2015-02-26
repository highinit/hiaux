#include "HttpServer.h"

CustomProtocolInfo::CustomProtocolInfo(const boost::function<CustomParserPtr(HttpRequestPtr) > &_parserBuilder,
				const boost::function<void(HttpConnectionPtr, CustomRequestPtr)> &_handler,
				const std::string &_handshake_message):
			parserBuilder(_parserBuilder),
			handler(_handler),
			handshake_message(_handshake_message) {
					
}

HttpServer::HttpServer(
	ResponseInfo _resp_info,
	boost::function<void(HttpConnectionPtr, HttpRequestPtr)> _request_hdl,
	int _port):
	
	m_resp_info(_resp_info),
	last_cleanup(0)
{
	
	m_listen_socket = startListening(_port);
	
	m_request_hdl = _request_hdl;
	
	m_events_watcher.reset(new EventWatcher(
			boost::bind(&HttpServer::onRead, this, _1, _2),
			boost::bind(&HttpServer::onWrite, this, _1, _2),
			boost::bind(&HttpServer::onError, this, _1, _2),
			boost::bind(&HttpServer::onAccept, this, _1, _2)));
	
	m_events_watcher->addSocket(m_listen_socket, HI_READ | HI_ACCEPT, NULL);
}

void HttpServer::addCustomProtocol(const std::string &_protocol,
						const CustomProtocolInfo &_info) {
	
	m_customProtocols.insert(make_pair(_protocol, _info));
}

void HttpServer::performAccept(int _sock_fd) {
		
	struct sockaddr_in cli_addr;
	size_t clilen = sizeof(cli_addr);
	int accepted_socket = accept(_sock_fd, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);

	while (accepted_socket > 0) {

		setSocketBlock(accepted_socket, false);
	
		HttpConnectionPtr connection(new HttpConnection(accepted_socket, m_resp_info, boost::bind(&HttpServer::onSendResponse, this, _1, _2),
														boost::bind(&HttpServer::onSendCustomResponse, this, _1, _2),
														boost::bind(&HttpServer::getCustomParser, this, _1, _2, _3)));
	
		m_reading_connections.insert(std::pair<int, HttpConnectionPtr>(connection->sock, connection));
			
		m_events_watcher->addSocket(connection->sock, HI_READ, NULL);
		accepted_socket = accept(_sock_fd, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);
	}
	
	if (errno == EINTR)
		performAccept(_sock_fd);
}

void HttpServer::onError(int _sock, void *_opaque_info) {
	
	killConnection(_sock);
}

void HttpServer::handleWaitingRequests(HttpConnectionPtr _conn) {
	
	if (!_conn->waiting_last_handling) {
	
		//m_events_watcher->delSocket(_sock);
		//m_reading_connections.erase(it);
	
		// http
		if (!_conn->custom_protocol && _conn->http_requests.size() != 0) {
	
			_conn->waiting_last_handling = true;
			HttpRequestPtr request = _conn->http_requests.front();
			_conn->http_requests.pop();
			if (!_conn->checkUpgrade(request))
				m_request_hdl(_conn, request);
		}
		// custom protocol
		else if (_conn->custom_requests.size() != 0) {
		
			_conn->waiting_last_handling = true;
			CustomRequestPtr request = _conn->custom_requests.front();
			_conn->custom_requests.pop();
			runCustomHandler(_conn, request);
		}
	}
}

void HttpServer::performRecv(int _sock) {
		
	try {
	
		hiaux::hashtable<int, HttpConnectionPtr>::iterator it = m_reading_connections.find(_sock);
		if (it == m_reading_connections.end()) {
			m_events_watcher->delSocket(_sock);
			return;
		}
	
		HttpConnectionPtr connection = it->second;
	
		connection->performRecv();
	
		if (!connection->notDead()) {
				
			m_events_watcher->delSocket(_sock);
			m_reading_connections.erase(it);
			return;
		}
		
		handleWaitingRequests(connection);
		
	}
	catch (std::bad_alloc e) {
		
		std::cout << "HttpServer::onRead bad_aloc\n";
		killConnection(_sock);
	}
	catch (RequestParsingEx e) {
		
		std::cout << "HttpServer::onRead RequestParsingEx\n"; 
		killConnection(_sock);
	}
	catch (...) {
		
		std::cout << "HttpServer::onRead exception\n";
		killConnection(_sock);
	}
}

void HttpServer::performSend(int _sock) {
	
	try {
		hiaux::hashtable<int, HttpConnectionPtr>::iterator it = m_reading_connections.find(_sock);
		if (it == m_reading_connections.end()) {
			return;
		}
	
		if (!it->second->performSend()) {

			if (!it->second->notDead()) {
		
				m_events_watcher->delSocket(_sock);
				m_reading_connections.erase( it );
				return;
			}
		}
	}
	catch (...) {
		
		std::cout << "HttpServer::onWrite exception\n";
		killConnection(_sock);
	}
}

void HttpServer::onRead(int _sock, void *_opaque_info) {
	
	performRecv(_sock);
}

void HttpServer::onWrite(int _sock, void *_opaque_info) {
	
	performSend(_sock);
}

void HttpServer::onAccept(int _sock, void *_opaque_info) {
	
	performAccept(_sock);
}

void HttpServer::killConnection(int _sock) {
	
	m_events_watcher->delSocket(_sock);
	hiaux::hashtable<int, HttpConnectionPtr>::iterator it = m_reading_connections.find(_sock);
	if (it == m_reading_connections.end()) {
		
		return;
	}
	m_reading_connections.erase( it );
}

void HttpServer::sendResponse(HttpConnectionPtr _conn, const HttpResponse &_resp) {
	
	m_resp_queue.push(std::make_pair(_conn, _resp));
}

void HttpServer::onSendResponse(int _sock, const HttpResponse &_resp) {
	
	hiaux::hashtable<int, HttpConnectionPtr>::iterator it = m_reading_connections.find(_sock);
	if (it == m_reading_connections.end())
		return;
	
	m_resp_queue.push(std::make_pair(it->second, _resp));
}

void HttpServer::onSendCustomResponse(int _sock, const std::string &_resp) {
	
	hiaux::hashtable<int, HttpConnectionPtr>::iterator it = m_reading_connections.find(_sock);
	if (it == m_reading_connections.end())
		return;
	
	m_custom_resp_queue.push(std::make_pair(it->second, _resp));
}

void HttpServer::handleResponse (HttpConnectionPtr _conn) {
	
	_conn->waiting_last_handling = false;
	
	if (!_conn->performSend()) {

		if (!_conn->notDead()) {
		
			m_events_watcher->delSocket(_conn->sock);
			m_reading_connections.erase( m_reading_connections.find(_conn->sock) );
			return;
		}
		
		m_events_watcher->enableEvents(_conn->sock, HI_READ | HI_WRITE);
	}
	
	handleWaitingRequests(_conn);
}

CustomParserPtr HttpServer::getCustomParser(const std::string &_protocol, const HttpRequestPtr &_req, std::string &_handshake) {
	
	std::map<std::string,  CustomProtocolInfo>::iterator it = m_customProtocols.find(_protocol);
	
	if (it == m_customProtocols.end())
		return CustomParserPtr();
	
	_handshake = it->second.handshake_message;
	
	return it->second.parserBuilder(_req);
}

void HttpServer::cleanUpDeadConnections() {
	
	uint64_t now = time(0);
	
	if (now - last_cleanup < 5)
		return;
	
	hiaux::hashtable<int, HttpConnectionPtr>::iterator it = m_reading_connections.begin();
	hiaux::hashtable<int, HttpConnectionPtr>::iterator end = m_reading_connections.end();
	
	while (it != end) {
		
		if (!it->second->notDead()) {
			
			m_events_watcher->delSocket(it->second->sock);
			m_reading_connections.erase(it);
		}
		it++;
	}
}

void HttpServer::handleEvents() {
	
	try {
	
		m_events_watcher->handleEvents();
		
		{
	
			while (!m_resp_queue.empty()) {
		
				std::pair<HttpConnectionPtr, HttpResponse> resp_context = m_resp_queue.front();
				resp_context.first->addResponse(resp_context.second);
				handleResponse(resp_context.first);
				m_resp_queue.pop();
			}
	
			while (!m_custom_resp_queue.empty()) {
				
				std::pair<HttpConnectionPtr, std::string> resp_context = m_custom_resp_queue.front();
				resp_context.first->addCustomResponse(resp_context.second);
				handleResponse(resp_context.first);
				m_custom_resp_queue.pop();
			}
		}
	
		cleanUpDeadConnections();
	}
	catch (...) {
		
		std::cout << "HttpServer::handleEvents exception\n";
	}
}

void HttpServer::runCustomHandler(HttpConnectionPtr _conn, CustomRequestPtr _req) {
	
	std::map<std::string, CustomProtocolInfo>::iterator it = m_customProtocols.find(_conn->custom_protocol_id);
	
	if (it == m_customProtocols.end()) {

		return;
	}
	
	it->second.handler(_conn, _req);
}

