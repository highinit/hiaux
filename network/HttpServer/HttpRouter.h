#ifndef _HTTP_ROUTER_H_
#define _HTTP_ROUTER_H_

#include "Connection.h"
#include "Request.h"

#include <boost/function.hpp>
#include <string>
#include <vector>

class HttpRouter {

	// path -> handler
	std::map<std::string, boost::function<void(HttpConnectionPtr, HttpRequestPtr)> > m_routes;

public:

	HttpRouter();
	virtual ~HttpRouter();

	void addRoute(const std::string &_path, const boost::function<void(HttpConnectionPtr, HttpRequestPtr)> &_handler);
	void handle(HttpConnectionPtr _conn, HttpRequestPtr _req);
};

#endif
