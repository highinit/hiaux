#include "HttpRouter.h"

HttpRouter::HttpRouter() {
	
}

HttpRouter::~HttpRouter() {
	
}

void HttpRouter::addRoute(
	const std::string &_path,
	const boost::function<void(HttpConnectionPtr, HttpRequestPtr)> &_handler,
	std::vector<std::string> _headers) {
	
	RouteInfo info;
	info.handler = _handler;
	info.headers = _headers;
	
	m_routes[ _path ] = info;
}

void HttpRouter::handle(HttpConnectionPtr _conn, HttpRequestPtr _req) {
	
	for (size_t i = _req->path.size(); i >= 1; i--) {
		
		auto it = m_routes.find( _req->path.substr(0, i) );
		if (it != m_routes.end()) {
			it->second.handler(_conn, _req);
			
			for (int h = 0; h<it->second.headers.size(); h++)
				_conn->addHeader( it->second.headers[h] );
			
			return;
		}
		
	}
}
