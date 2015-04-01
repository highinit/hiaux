#include "HttpRouter.h"

HttpRouter::HttpRouter() {
	
}

HttpRouter::~HttpRouter() {
	
}

void HttpRouter::addRoute(const std::string &_path, const boost::function<void(HttpConnectionPtr, HttpRequestPtr)> &_handler) {
	
	m_routes[ _path ] = _handler;
}

void HttpRouter::handle(HttpConnectionPtr _conn, HttpRequestPtr _req) {
	
	for (size_t i = _req->path.size(); i >= 1; i--) {
		
		auto it = m_routes.find( _req->path.substr(0, i) );
		if (it != m_routes.end()) {
			it->second(_conn, _req);
			return;
		}
		
	}
}
