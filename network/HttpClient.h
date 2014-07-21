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
	
	bool callSimple (const std::string &_url, std::string &_resp);
	bool callPost (const std::string &_url, const std::string &_postdata, std::string &_resp);
};

typedef boost::shared_ptr<HttpClient> HttpClientPtr;

size_t crawl_function_pt(void *ptr, size_t size, size_t nmemb, std::string *stream);


#endif
