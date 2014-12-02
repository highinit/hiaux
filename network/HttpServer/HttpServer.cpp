#include "HttpServer.h"

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

void HttpServer::onAccept(int _sock_fd, void *_opaque_info) {
	
	//std::cout << "onAccept\n";
	
	struct sockaddr_in cli_addr;
	size_t clilen = sizeof(cli_addr);
	int accepted_socket = accept(_sock_fd, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);

	while (accepted_socket > 0) {

		setSocketBlock(accepted_socket, false);
	
		HttpConnectionPtr connection(new HttpConnection(accepted_socket, m_resp_info, boost::bind(&HttpServer::onSendResponse, this, _1, _2)));
	
		m_reading_connections.insert(std::pair<int, HttpConnectionPtr>(connection->sock, connection));
			
		m_events_watcher->addSocket(connection->sock, HI_READ, NULL);
		accepted_socket = accept(_sock_fd, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);
	}
}

void HttpServer::onError(int _sock, void *_opaque_info) {
	
	//std::cout << "HttpServer::onError\n";
	m_events_watcher->delSocket(_sock);
	m_reading_connections.erase( m_reading_connections.find(_sock) );
}

void HttpServer::onRead(int _sock, void *_opaque_info) {
	
	hiaux::hashtable<int, HttpConnectionPtr>::iterator it = m_reading_connections.find(_sock);
	if (it == m_reading_connections.end()) {
		m_events_watcher->delSocket(_sock);
		return;
	}
	
	HttpConnectionPtr connection = it->second;
	
	connection->performRecv();
	if (connection->request_finished) {
		
		//std::cout << "HttpServer:: onRequest " << connection->request->url << std::endl;
		
		connection->requests.push(connection->request);
		connection->request.reset(new HttpRequest);
		connection->request_finished = false;
		
		
	}
	
	if (!connection->notDead()) {
		
		//std::cout << "HttpServer::onRead connection dead\n";
		
		m_events_watcher->delSocket(_sock);
		m_reading_connections.erase(it);
		
		return;
	}
	
	if (!connection->waiting_last_handling && connection->requests.size() != 0) {
		
		//m_events_watcher->delSocket(_sock);
		//m_reading_connections.erase(it);
		
		connection->waiting_last_handling = true;
		HttpRequestPtr request = connection->requests.front();
		connection->requests.pop();
		m_launcher->addTask(NEW_LAUNCHER_TASK4(&HttpServer::workerTask, this, connection, request));
	}
}

void HttpServer::onWrite(int _sock, void *_opaque_info) {
	
	//std::cout << "HttpServer::onWrite\n";
	
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

TaskLauncher::TaskRet HttpServer::eventLoop() {
	
	while (m_is_running) {
		
		m_events_watcher->handleEvents();
		
		hLockTicketPtr ticket = resp_lock.lock();
		
		while (!m_resp_queue.empty()) {
			
			std::pair<HttpConnectionPtr, HttpResponse> resp_context = m_resp_queue.front();
			resp_context.first->addResponse(resp_context.second);
			handleResponse(resp_context.first);
			m_resp_queue.pop();
		}
		
		cleanUpDeadConnections();
	}
	
	return TaskLauncher::NO_RELAUNCH;
}

TaskLauncher::TaskRet HttpServer::workerTask(HttpConnectionPtr _conn, HttpRequestPtr _req) {

	m_request_hdl(_conn, _req);
	return TaskLauncher::NO_RELAUNCH;
}

