#include "Request.h"

namespace hiaux {

HttpRequest::HttpRequest(const std::string &_url):
url(_url) {

}

bool HttpRequest::getField(const std::string &_key, std::string &_value) {
	
	std::map<std::string, std::string>::iterator it = values_GET.find(_key);
	
	if (it != values_GET.end()) {
		_value = it->second;
		return true;
	} else
		return false;
}

bool HttpRequest::getCookie(const std::string &_name, std::string &_value) {
	
	std::map<std::string, std::string>::iterator it = cookies.find(_name);
	if (it == cookies.end())
		return false;
	_value = it->second;
	return true;
}

}
