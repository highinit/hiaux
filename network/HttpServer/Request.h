#ifndef _REQUEST_H_
#define _REQUEST_H_

#include "hiconfig.h"
#include <boost/shared_ptr.hpp>
#include "hiaux/structs/hashtable.h"
#include "hiaux/strings/string_utils.h"

class HttpRequest {
public:
	std::string url;
	std::string path;
	
	std::map<std::string, std::string> headers;
	std::map<std::string, std::string> values_GET;
	std::map<std::string, std::string> cookies;
	std::string body;
	
	HttpRequest() { }
	HttpRequest(const std::string &_url);
	bool getField(const std::string &_key, std::string &_value);
	bool getCookie(const std::string &_name, std::string &_value);
};

typedef boost::shared_ptr<HttpRequest> HttpRequestPtr;

#endif
