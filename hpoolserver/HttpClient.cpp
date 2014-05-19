#include "HttpClient.h"

HttpClient::HttpClient() {
	m_curl = curl_easy_init();
	curl_easy_setopt(m_curl, CURLOPT_DNS_CACHE_TIMEOUT, 3600);	
}

HttpClient::~HttpClient() {
	curl_easy_cleanup(m_curl);
}

size_t crawl_function_pt(void *ptr, size_t size, size_t nmemb, std::string *stream) {
	std::string bf = *stream + std::string((char*)ptr);
	stream->clear();
	*stream = bf;
	return size*nmemb;
}

void HttpClient::callSimple(const std::string &_url, std::string &_resp) {
	curl_easy_setopt(m_curl, CURLOPT_URL, _url.c_str());
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, crawl_function_pt);
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &_resp);
	curl_easy_setopt(m_curl, CURLOPT_ENCODING, "UTF-8");
	curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 10);
	curl_easy_setopt(m_curl, CURLOPT_USERAGENT, "hiaux HttpClient");
	CURLcode res = curl_easy_perform(m_curl);

	if (res!=0) {
		_resp += "HttpApiClient::call error happened";
	}
}
