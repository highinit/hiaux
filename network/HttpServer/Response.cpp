#include "Response.h"

HttpResponse::HttpResponse(int _code, const std::string &_body):
code(_code),
body(_body) {
	
}

void HttpResponse::setCookie(const std::string &_name, const std::string &_value) {

	headers.push_back(std::string("Set-Cookie: ") + _name + "=" + _value + "; expires=Sat, 31 Dec 2039 23:59:59 GMT");
}

void HttpResponse::addHeader(const std::string &_header) {
	
	headers.push_back(_header);
}
