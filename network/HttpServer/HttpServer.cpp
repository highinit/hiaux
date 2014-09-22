#include "HttpServer.h"

HttpServer::HttpServer(TaskLauncherPtr launcher,
					const ResponseInfo &_resp_info,
					boost::function<void(HttpConnectionPtr,
										HttpRequestPtr)> _request_hdl,
					int _port):
			m_resp_info(_resp_info) {

	m_is_running = true;
	m_listen_socket = startListening(_port);
	m_launcher = launcher;
	m_request_hdl = _request_hdl;
	
	m_events_watcher.reset(new EventWatcher(
			boost::bind(&HttpServer::onRead, this, _1, _2),
			boost::bind(&HttpServer::onWrite, this, _1, _2),
			boost::bind(&HttpServer::onError, this, _1, _2),
			boost::bind(&HttpServer::onAccept, this, _1, _2)));
	
	m_events_watcher->addSocketAccept(m_listen_socket, NULL);
	m_launcher->addTask(NEW_LAUNCHER_TASK2(&HttpServer::eventLoop, this));
}

void HttpServer::onRead(int _sock, void *_opaque_info) {
	
	hiaux::hashtable<int, HttpConnectionPtr>::iterator it = m_reading_connections.find(_sock);
	if (it == m_reading_connections.end()) {
		m_events_watcher->delSocket(_sock, NULL);
		return;
	}
	
	HttpConnectionPtr connection = it->second;
	
	connection->performRecv();
	if (connection->request_finished) {
		
		m_events_watcher->delSocket(_sock, NULL);
		m_reading_connections.erase(it);
		m_launcher->addTask(NEW_LAUNCHER_TASK4(&HttpServer::workerTask, this, connection, connection->request));
	} else
		if (!connection->notDead()) {
			
			m_events_watcher->delSocket(_sock, NULL);
			m_reading_connections.erase(it);
		}
}

void HttpServer::onWrite(int _sock, void *_opaque_info) {
	
	std::cout << "HttpServer::onWrite\n";
}

void HttpServer::onError(int _sock, void *_opaque_info) {
	
	std::cout << "HttpServer::onError\n";
}

void HttpServer::onAccept(int _sock_fd, void *_opaque_info) {
	
	struct sockaddr_in cli_addr;
	size_t clilen = sizeof(cli_addr);
	int accepted_socket = accept(_sock_fd, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);

	if (accepted_socket < 0)
		return;
	
	HttpConnectionPtr connection(new HttpConnection(accepted_socket, m_resp_info));
	
	m_reading_connections.insert(std::pair<int, HttpConnectionPtr>(connection->sock, connection));
			
	m_events_watcher->addSocketRead(connection->sock, NULL);
}

TaskLauncher::TaskRet HttpServer::eventLoop() {
	
	while (m_is_running) {
		m_events_watcher->handleEvents();
	}
	
	return TaskLauncher::NO_RELAUNCH;
}

TaskLauncher::TaskRet HttpServer::workerTask(HttpConnectionPtr _conn, HttpRequestPtr _req) {

	m_request_hdl(_conn, _req);
}
