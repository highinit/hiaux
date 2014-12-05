#include "HttpServer.h"

CustomProtocolInfo::CustomProtocolInfo(const boost::function<CustomParserPtr(HttpRequestPtr) > &_parserBuilder,
				const boost::function<void(HttpConnectionPtr, CustomRequestPtr)> &_handler,
				const std::string &_handshake_message):
			parserBuilder(_parserBuilder),
			handler(_handler),
			handshake_message(_handshake_message) {
					
}

HttpServer::HttpServer(TaskLauncherPtr launcher,
					const ResponseInfo &_resp_info,
					boost::function<void(HttpConnectionPtr,
										HttpRequestPtr)> _request_hdl,
					int _port):
			m_resp_info(_resp_info),
			last_cleanup(0) {

	m_is_running = true;
	
	m_listen_socket = startListening(_port);
	
	m_launcher = launcher;
	m_request_hdl = _request_hdl;
	
	m_events_watcher.reset(new EventWatcher(
			boost::bind(&HttpServer::onRead, this, _1, _2),
			boost::bind(&HttpServer::onWrite, this, _1, _2),
			boost::bind(&HttpServer::onError, this, _1, _2),
			boost::bind(&HttpServer::onAccept, this, _1, _2)));
	
	m_events_watcher->addSocket(m_listen_socket, HI_READ | HI_ACCEPT, NULL);
	m_launcher->addTask(NEW_LAUNCHER_TASK2(&HttpServer::eventLoop, this));
}

HttpServer::HttpServer(TaskLauncherPtr launcher,
			const ResponseInfo &_resp_info,
			boost::function<void(HttpConnectionPtr,
								HttpRequestPtr)> _request_hdl,
			const std::string &_localsocket):
			m_resp_info(_resp_info) {
	
	m_is_running = true;

	m_listen_socket = startListening(_localsocket);

	m_launcher = launcher;
	m_request_hdl = _request_hdl;

	m_events_watcher.reset(new EventWatcher(
		boost::bind(&HttpServer::onRead, this, _1, _2),
		boost::bind(&HttpServer::onWrite, this, _1, _2),
		boost::bind(&HttpServer::onError, this, _1, _2),
		boost::bind(&HttpServer::onAccept, this, _1, _2)));

	m_events_watcher->addSocket(m_listen_socket, HI_READ | HI_ACCEPT, NULL);
	m_launcher->addTask(NEW_LAUNCHER_TASK2(&HttpServer::eventLoop, this));
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
}

void HttpServer::onError(int _sock, void *_opaque_info) {
	
	killConnection(_sock);
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
		
			//std::cout << "HttpServer::onRead connection dead\n";
		
			m_events_watcher->delSocket(_sock);
			m_reading_connections.erase(it);
		
			return;
		}
	
		if (!connection->waiting_last_handling) {
		
			//m_events_watcher->delSocket(_sock);
			//m_reading_connections.erase(it);
		
			// http
			if (!connection->custom_protocol && connection->http_requests.size() != 0) {
		
				connection->waiting_last_handling = true;
				HttpRequestPtr request = connection->http_requests.front();
				connection->http_requests.pop();
				if (!connection->checkUpgrade(request))
					m_launcher->addTask(NEW_LAUNCHER_TASK4(&HttpServer::httpWorkerTask, this, connection, request));
			}
			// custom protocol
			else if (connection->custom_requests.size() != 0) {
			
				connection->waiting_last_handling = true;
				CustomRequestPtr request = connection->custom_requests.front();
				connection->custom_requests.pop();
				m_launcher->addTask(NEW_LAUNCHER_TASK4(&HttpServer::customWorkerTask, this, connection, request));
			}
		}
	}
	catch (std::bad_alloc e) {
		
		std::cout << "HttpServer::onRead bad_aloc\n";
	}
	catch (RequestParsingEx e) {
		
		std::cout << "HttpServer::onRead RequestParsingEx\n"; 
		killConnection(_sock);
	}
	catch (...) {
		
		killConnection(_sock);
	}
}

void HttpServer::performSend(int _sock) {
	
	try {
		hiaux::hashtable<int, HttpConnectionPtr>::iterator it = m_reading_connections.find(_sock);
		if (it == m_reading_connections.end()) {
			//std::cout << "HttpServer::onWrite conneciton not found\n";
			return;
		}
	
		if (!it->second->performSend()) {

			if (!it->second->notDead()) {
		
				//std::cout << "HttpServer::handleResponse connection dead\n";
		
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
	
	//std::cout << "HttpServer::killConnection\n";
	
	m_events_watcher->delSocket(_sock);
	hiaux::hashtable<int, HttpConnectionPtr>::iterator it = m_reading_connections.find(_sock);
	if (it == m_reading_connections.end()) {
		
		return;
	}
	m_reading_connections.erase( it );
}

void HttpServer::sendResponse(HttpConnectionPtr _conn, const HttpResponse &_resp) {
	
	hLockTicketPtr ticket = resp_lock.lock();
	m_resp_queue.push(std::make_pair(_conn, _resp));
}

void HttpServer::onSendResponse(int _sock, const HttpResponse &_resp) {
	
	hLockTicketPtr ticket = resp_lock.lock();
	
	//std::cout << "HttpServer::onSendResponse\n";
	
	hiaux::hashtable<int, HttpConnectionPtr>::iterator it = m_reading_connections.find(_sock);
	if (it == m_reading_connections.end())
		return;
	
	m_resp_queue.push(std::make_pair(it->second, _resp));
}

void HttpServer::onSendCustomResponse(int _sock, const std::string &_resp) {
	
	hLockTicketPtr ticket = resp_lock.lock();
	
	hiaux::hashtable<int, HttpConnectionPtr>::iterator it = m_reading_connections.find(_sock);
	if (it == m_reading_connections.end())
		return;
	
	m_custom_resp_queue.push(std::make_pair(it->second, _resp));
}

void HttpServer::handleResponse (HttpConnectionPtr _conn) {
	
	_conn->waiting_last_handling = false;
	
	if (!_conn->performSend()) {

		if (!_conn->notDead()) {
		
			//std::cout << "HttpServer::handleResponse connection dead\n";
		
			m_events_watcher->delSocket(_conn->sock);
			m_reading_connections.erase( m_reading_connections.find(_conn->sock) );
			return;
		}
		
		m_events_watcher->enableEvents(_conn->sock, HI_READ | HI_WRITE);
	}
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
			hLockTicketPtr ticket = resp_lock.lock();
	
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
		
	}
}

TaskLauncher::TaskRet HttpServer::eventLoop() {
	
	while (m_is_running) {
		
		handleEvents();
	}
	
	return TaskLauncher::NO_RELAUNCH;
}

TaskLauncher::TaskRet HttpServer::customWorkerTask(HttpConnectionPtr _conn, CustomRequestPtr _req) {
	
	std::map<std::string, CustomProtocolInfo>::iterator it = m_customProtocols.find(_conn->custom_protocol_id);
	
	if (it == m_customProtocols.end())
		return TaskLauncher::NO_RELAUNCH;
	
	it->second.handler(_conn, _req);
	
	return TaskLauncher::NO_RELAUNCH;
}

TaskLauncher::TaskRet HttpServer::httpWorkerTask(HttpConnectionPtr _conn, HttpRequestPtr _req) {

	m_request_hdl(_conn, _req);
	return TaskLauncher::NO_RELAUNCH;
}

