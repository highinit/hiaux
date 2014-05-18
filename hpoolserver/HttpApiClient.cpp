#include "HttpApiClient.h"
#include <iostream>

HttpApiClient::HttpApiClient(const std::string &_url,
				const std::string &_userid,
				const std::string &_key):
	m_url(_url),
	m_api_userid(_userid),
	m_api_key(_key),
	m_is_auth_info_set(true) {
	m_curl = curl_easy_init();
	curl_easy_setopt(m_curl, CURLOPT_DNS_CACHE_TIMEOUT, 3600);			
}
				
HttpApiClient::HttpApiClient(const std::string &_url):
	m_url(_url),
	m_is_auth_info_set(false) {
	m_curl = curl_easy_init();
	curl_easy_setopt(m_curl, CURLOPT_DNS_CACHE_TIMEOUT, 3600);
}

HttpApiClient::~HttpApiClient() {
	curl_easy_cleanup(m_curl);
}

void HttpApiClient::buildRequestUrl(const std::string &_method,
					const hiaux::hashtable<std::string, std::string> &_get_params,
					std::string &_req) const {
	std::string ts = uint64_to_string(time(0));
	std::string sign_raw = _method + ts + m_api_key;
	unsigned char sign[21];
	sha1::calc(sign_raw.c_str(), sign_raw.size(), sign);

	char sign_hex[41];
	sha1::toHexString(sign, sign_hex);

	_req = m_url + "?method=" + _method + "&api_userid=" + m_api_userid + "&ts=" + ts + "&sign=" + sign_hex;

	hiaux::hashtable<std::string, std::string>::const_iterator it = _get_params.begin();
	hiaux::hashtable<std::string, std::string>::const_iterator end = _get_params.end();
	while (it != end) {
		_req += "&" + it->first + "=" + it->second;
		it++;
	}
}

size_t crawl_function_pt(void *ptr, size_t size, size_t nmemb, std::string *stream)
{
	std::string bf = *stream + std::string((char*)ptr);
	stream->clear();
	*stream = bf;
	return size*nmemb;
}

void HttpApiClient::call(const std::string &_method,
						const hiaux::hashtable<std::string, std::string> &_get_params,
						std::string &_resp) const {
	std::string req_url;
	buildRequestUrl(_method, _get_params, req_url);
	
	curl_easy_setopt(m_curl, CURLOPT_URL, req_url.c_str());
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, crawl_function_pt);
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &_resp);
	curl_easy_setopt(m_curl, CURLOPT_ENCODING, "UTF-8");
	curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 10);
	curl_easy_setopt(m_curl, CURLOPT_USERAGENT, "hiaux HttpApiClient");
	CURLcode res = curl_easy_perform(m_curl);

	if (res!=0) {
		_resp += "HttpApiClient::call error happened";
	}
	
}
