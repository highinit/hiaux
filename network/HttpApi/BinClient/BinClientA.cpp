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

void BinClientA::performRecv(int _sock) {
	
	try {
	
		hiaux::hashtable<int, ConnectionPtr>::iterator it = m_connections.find(_sock);
		if (it == m_connections.end())
			return;
	
		it->second->performRecv();
	} 
	catch (LostConnectionEx e) {
		
		onLostConnection(_sock);
	}
	catch (CannotHandShakeEx e) {
		
		std::cout << "BinClientA::onRead CannotHandShakeEx\n";
		onLostConnection(_sock);
	}
	catch (ResponseParsingEx e) {
		
		std::cout << "BinClientA::onRead ResponseParsingEx\n";
		onLostConnection(_sock);
	}
	catch (...) {
		
	}
}

void BinClientA::performSend(int _sock) {
	
	try {
		
		hiaux::hashtable<int, ConnectionPtr>::iterator it = m_connections.find(_sock);
		if (it == m_connections.end())
			return;
		
		it->second->performSend();
	}
	catch (LostConnectionEx e) {
		
		onLostConnection(_sock);
	}
	catch (CannotHandShakeEx e) {
		
		onLostConnection(_sock);
	}
	catch (...) {
		
	}
}

void BinClientA::onRead(int _sock, void *_opaque_info) {

	//std::cout << "__BinClientA::onRead\n";
	performRecv(_sock);
}

void BinClientA::onWrite(int _sock, void *_opaque_info) {
	
	//std::cout << "__BinClientA::onWrite\n";
	performSend(_sock);
}

void BinClientA::onAccept(int _sock, void *_opaque_info) {
	
	//std::cout << "__BinClientA::onAccept\n";
}

void BinClientA::onError(int _sock, void *_opaque_info) {
	
	//std::cout << "__BinClientA::onError\n";
	onLostConnection(_sock);
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

void BinClientA::onLostConnection(int _sock) {
	
	std::cout << "BinClientA::onLostConnection\n";
	
	try {
		
		hiaux::hashtable<int, ConnectionPtr>::iterator it = m_connections.find(_sock);
		if (it == m_connections.end())
			return;	
		
		m_events_watcher->delSocket(_sock);
		m_connections.erase(it);
		
		establishNewConnection();
		
	}
	catch (CannotConnectEx e) {
		
		std::cout << "BinClientA::onLostConnection CannotConnectEx\n";
	}
	catch (...) {
		
	}
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
}

void BinClientA::call(const std::string &_method,
							const std::map<std::string, std::string> &_params,
							const boost::function<void(bool, const std::string &)> &_onFinished) {
	
	std::string req;
	buildRequest(_method, _params, req);
	
	hLockTicketPtr ticket = lock.lock();
	m_new_requests.push(RequestPtr(new Request(req, _onFinished)));
}

void BinClientA::putRequestsToConnections() {
	
	//std::cout << "BinClientA::putRequestsToConnections\n";
	
	// put requests to connecitons
	hiaux::hashtable<int, ConnectionPtr>::iterator it = m_connections.begin();
	hiaux::hashtable<int, ConnectionPtr>::iterator end = m_connections.end();
	
	
	while (!m_new_requests.empty()) {
		
		RequestPtr req;
		
		
		hLockTicketPtr ticket = lock.lock();
		req = m_new_requests.front();
		
		
		if (it->second->state == Connection::JUST_HANDSHAKED) {
			
			//std::cout << "it->second->state == Connection::JUST_HANDSHAKED\n";
			it->second->addRequest(req);
			m_new_requests.pop();
			
			//ticket->unlock();
			
			performSend(it->first);
		}
		else if (it->second->state == Connection::ACTIVE) {
			
			//std::cout << "it->second->state == Connection::ACTIVE\n";
			
			it->second->addRequest(req);
			m_new_requests.pop();
		}

		it++;
		
		if (it == end)
			return;
			//it = m_connections.begin();
	}
}

void BinClientA::handleEvents() {
	
	try {
		
		//std::cout << "m_events_watcher->handleEvents\n";
		
		if (m_connections.size() == 0) {
			//std::cout << "reinitConnections\n";
			reinitConnections();
		}
		
		putRequestsToConnections();
		
		//std::cout << "putRequestsToConnections done\n";
		
		m_events_watcher->handleEvents();
	}
	catch (...) {
		
		std::cout << "BinClientA::handleEvents Exception\n";
	}
}


}
}
