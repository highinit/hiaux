#include "BinClientA.h"

namespace hiapi {

namespace client {

BinClientA::BinClientA(BinClientA::Mode _mode, const std::string &_endpoint, size_t _max_connections):
	 m_mode(_mode),
	 m_endpoint(_endpoint),
	 m_max_connections(_max_connections) {

	m_events_watcher.reset(new EventWatcher(
		boost::bind(&BinClientA::onRead, this, _1, _2),
		boost::bind(&BinClientA::onWrite, this, _1, _2),
		boost::bind(&BinClientA::onError, this, _1, _2),
		boost::bind(&BinClientA::onAccept, this, _1, _2)));

}

BinClientA::~BinClientA() {
	
}

void BinClientA::onRead(int _sock, void *_opaque_info) {
	
}

void BinClientA::onWrite(int _sock, void *_opaque_info) {
	
}

void BinClientA::onError(int _sock, void *_opaque_info) {
	
}

void BinClientA::onAccept(int _sock, void *_opaque_info) {
	
}

void BinClientA::establishNewConnection() {
	
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
							boost::function<void(bool, const std::string &)> &_onFinished) {
	
	std::string req;
	buildRequest(_method, _params, req);
	
	hLockTicketPtr ticket = lock.lock();
	m_new_requests.push(RequestPtr(new Request(req, _onFinished)));
}

void BinClientA::handleEvents() {
	
}


}
}
