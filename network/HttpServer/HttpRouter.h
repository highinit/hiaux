#ifndef _HTTP_ROUTER_H_
#define _HTTP_ROUTER_H_

#include "Connection.h"
#include "Request.h"

#include <boost/function.hpp>
#include <string>
#include <vector>

class HttpRouter {

	class RouteInfo {
	public:
		boost::function<void(HttpConnectionPtr, HttpRequestPtr)> handler;
		std::vector<std::string> headers;
	};

	// path -> handler
	std::map<std::string, RouteInfo> m_routes;

public:

	HttpRouter();
	virtual ~HttpRouter();

	void addRoute(
		const std::string &_path,
		const boost::function<void(HttpConnectionPtr, HttpRequestPtr)> &_handler,
		std::vector<std::string> _headers = std::vector<std::string>());
		
	void handle(HttpConnectionPtr _conn, HttpRequestPtr _req);
};

#endif
