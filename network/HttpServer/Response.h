#ifndef _RESPONSE_H_
#define _RESPONSE_H_

#include "hiconfig.h"
#include <boost/shared_ptr.hpp>

#include "hiaux/strings/string_utils.h"

#include <map>

class HttpResponse {
public:
	int code;
	std::string body;
	std::vector<std::string> headers;
	
	HttpResponse(int _code, const std::string &_body);
	
	void setCookie(const std::string &_name, const std::string &_value);
	void addHeader(const std::string &_header);
};

typedef boost::shared_ptr<HttpResponse> HttpResponsePtr;

#endif //_RESPONSE_H_
