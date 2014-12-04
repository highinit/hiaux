#ifndef _RESPONSE_H_
#define _RESPONSE_H_

#include "hiconfig.h"
#include <boost/shared_ptr.hpp>

#include "hiaux/strings/string_utils.h"

class HttpResponse {
public:
	int code;
	std::string body;
	
	HttpResponse(int _code, const std::string &_body);
};

typedef boost::shared_ptr<HttpResponse> HttpResponsePtr;

#endif //_RESPONSE_H_
