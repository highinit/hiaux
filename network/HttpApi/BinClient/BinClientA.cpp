#include "BinClientA.h"

namespace hiapi {

namespace client {

BinClientA::BinClientA(BinClientA::Mode _mode, const std::string &_ip, int _port, size_t _max_connections):
	m_mode(_mode),
	m_ip(_ip),
	m_port(_port),
	m_max_connections(_max_connections) {

	m_events_watcher.reset(new EventWatcher(
		boost::bind(&BinClientA::onRead, this, _1, _2),
		boost::bind(&BinClientA::onWrite, this, _1, _2),
		boost::bind(&BinClientA::onError, this, _1, _2),
		boost::bind(&BinClientA::onAccept, this, _1, _2)));

	reinitConnections();
}

BinClientA::~BinClientA() {
	
}

void BinClientA::reinitConnections() {
	
	for (size_t i = 0; i<m_max_connections; i++)
		establishNewConnection();
}

void BinClientA::performRecv(ConnectionPtr _conn) {
	
	try {
	
	/*	hiaux::hashtable<int, ConnectionPtr>::iterator it = m_connections.find(_sock);
		if (it == m_connections.end())
			return;
	*/
		_conn->performRecv();
	} 
	catch (LostConnectionEx e) {
		
		onLostConnection(_conn);
	}
	catch (CannotHandShakeEx e) {
		
		std::cout << "BinClientA::onRead CannotHandShakeEx\n";
		onLostConnection(_conn);
	}
	catch (ResponseParsingEx e) {
		
		std::cout << "BinClientA::onRead ResponseParsingEx\n";
		onLostConnection(_conn);
	}
	catch (...) {
		
		std::cout << "BinClientA::onRead exception\n";
	}
}

void BinClientA::performSend(ConnectionPtr _conn) {
	
	try {
		
//		hiaux::hashtable<int, ConnectionPtr>::iterator it = m_connections.find(_sock);
//		if (it == m_connections.end())
//			return;
		
		_conn->performSend();
	}
	catch (LostConnectionEx e) {
		
		onLostConnection(_conn);
	}
	catch (CannotHandShakeEx e) {
		
		onLostConnection(_conn);
	}
	catch (...) {
		
		std::cout << "BinClientA::performSend exception\n";
	}
}

void BinClientA::onRead(int _sock, void *_opaque_info) {

	//std::cout << "__BinClientA::onRead\n";
	std::map<int, ConnectionPtr>::iterator it = m_connections.find(_sock);
	if (it == m_connections.end())
		return;
		
	performRecv(it->second);
}

void BinClientA::onWrite(int _sock, void *_opaque_info) {
	
	hLockTicketPtr ticket = lock.lock();
	
	std::map<int, ConnectionPtr>::iterator it = m_connections.find(_sock);
	if (it == m_connections.end())
		return;
	
	ConnectionPtr conn = it->second;
	
	if (conn->state == Connection::ACTIVE) {
		
		if (conn->m_send_buffer.size()==0) {
		
			if (!m_new_requests.empty()) {
		
				RequestPtr req = m_new_requests.front();
				conn->addRequest(req);
				m_new_requests.pop();
			}
			else {
				m_free_connections[_sock] = conn;
				return;
			}		
		}
	}
	else { // connection waiting server handshake
		
		if (conn->m_send_buffer.size()==0) {
			
			m_free_connections[_sock] = conn;
			//return;
		}
	}
	
	performSend(conn);
}

void BinClientA::onAccept(int _sock, void *_opaque_info) {
	
	//std::cout << "__BinClientA::onAccept\n";
}

void BinClientA::onError(int _sock, void *_opaque_info) {
	
	//std::cout << "__BinClientA::onError\n";
	
	std::map<int, ConnectionPtr>::iterator it = m_connections.find(_sock);
	if (it == m_connections.end())
		return;
	
	onLostConnection(it->second);
}

void BinClientA::buildRequest(const std::string &_method, const std::map<std::string, std::string> &_params, std::string &_dump) {
	
	hiapi_client::RequestPb pb;
	pb.set_method(_method);
	
	std::map<std::string, std::string>::const_iterator it = _params.begin();
	std::map<std::string, std::string>::const_iterator end = _params.end();
	
	while (it != end) {
		
		hiapi_client::RequestParamPb *p_pb = pb.add_params();
		p_pb->set_key(it->first);
		p_pb->set_value(it->second);
		it++;
	}
	
	_dump = pb.SerializeAsString();
	
	if (_dump.find('\0') != std::string::npos)
		std::cout << "BinClientA::buildRequest contains NULL-terminator\n\n";
}

void BinClientA::call(const std::string &_method,
							const std::map<std::string, std::string> &_params,
							const boost::function<void(bool, const std::string &)> &_onFinished) {
	
	std::string req;
	buildRequest(_method, _params, req);
	
	hLockTicketPtr ticket = lock.lock();
	m_new_requests.push(RequestPtr(new Request(req, _onFinished)));
}

void BinClientA::callSigned (const std::string &_method, const std::map<std::string, std::string> &_params, const boost::function<void(bool, const std::string &)> &_onFinished) {
	
}

void BinClientA::putRequestsToFreeConnections() {
	
	hLockTicketPtr ticket = lock.lock();
	
	if (m_new_requests.empty())
		return;
	
	if (m_free_connections.size() == 0)
		return;
	
	RequestPtr req = m_new_requests.front();
	
	std::map<int, ConnectionPtr>::iterator it = m_free_connections.begin();
	std::map<int, ConnectionPtr>::iterator end = m_free_connections.end();
	
	while (it != end) {
		
		ConnectionPtr conn = it->second;
		
		if (conn->state == Connection::ACTIVE) {
		
			m_new_requests.pop();
			conn->addRequest(req);
			performSend(conn);
			m_free_connections.erase(it);
			return;
		}
		
		it++;
	}
}

void BinClientA::establishNewConnection() {
	
	try {
		int sock = connectSocket(m_ip, m_port);

		m_connections.insert(std::make_pair(sock, ConnectionPtr(new Connection( sock ))));
		m_events_watcher->addSocket(sock, HI_READ | HI_WRITE, NULL);
	
	} catch (CannotConnectEx e) {
		
		std::cout << "BinClientA::establishNewConnection CannotConnectEx\n";
	}
}

void BinClientA::removeConnection(ConnectionPtr _conn) {
	
	m_events_watcher->delSocket(_conn->m_sock);
	
	std::map<int, ConnectionPtr>::iterator it = m_connections.find(_conn->m_sock);
	if (it != m_connections.end())	
		m_connections.erase(it);
	
	it = m_free_connections.find(_conn->m_sock);
	if (it != m_free_connections.end())
		m_free_connections.erase(it);
}

void BinClientA::onLostConnection(ConnectionPtr _conn) {
	
	std::cout << "BinClientA::onLostConnection\n";
	
	try {
		
		removeConnection(_conn);
		
		establishNewConnection();
	}
	catch (CannotConnectEx e) {
		
		std::cout << "BinClientA::onLostConnection CannotConnectEx\n";
	}
	catch (...) {
		
		std::cout << "BinClientA::onLostConnection unknown exection\n";
	}
}

void BinClientA::checkKeepAlive() {
	
	uint64_t now = time(0);
	
	std::map<int, ConnectionPtr>::iterator it = m_connections.begin();
	std::map<int, ConnectionPtr>::iterator end = m_connections.end();
	
	while (it != end) {
		it->second->checkKeepAlive(now);
		it++;
	}
}

void BinClientA::handleEvents() {
	
	try {
		
		//std::cout << "m_events_watcher->handleEvents\n";
		
		if (m_connections.size() < m_max_connections) {
			//std::cout << "reinitConnections\n";
			establishNewConnection();
			//reinitConnections();
		}
		
		putRequestsToFreeConnections();
		//checkKeepAlive();
		
		//std::cout << "putRequestsToConnections done\n";
		
		m_events_watcher->handleEvents();
	}
	catch (...) {
		
		std::cout << "BinClientA::handleEvents Exception\n";
	}
}


}
}
