#ifndef _HTTPCLIENT_H_
#define _HTTPCLIENT_H_

#include "hiconfig.h"
#include "curl/curl.h"

#include <boost/shared_ptr.hpp>

class HttpClient {
	CURL *m_curl;
public:
	HttpClient();
	~HttpClient();
	
	void callSimple (const std::string &_url, std::string &_resp);
};

typedef boost::shared_ptr<HttpClient> HttpClientPtr;

#endif
