#include "Response.h"

HttpResponse::HttpResponse(int _code, const std::string &_body):
code(_code),
body(_body) {
	
}
