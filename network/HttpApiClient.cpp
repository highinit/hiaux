#include "HttpApiClient.h"
#include <iostream>

HttpApiClient::HttpApiClient(const std::string &_url,
				const std::string &_userid,
				const std::string &_key):
	m_url(_url),
	m_api_userid(_userid),
	m_api_key(_key),
	m_is_auth_info_set(true),
	m_http_cli(new HttpClient()) {		
}
				
HttpApiClient::HttpApiClient(const std::string &_url):
	m_url(_url),
	m_is_auth_info_set(false),
	m_http_cli(new HttpClient()) {
}

HttpApiClient::~HttpApiClient() {
}

void HttpApiClient::buildRequestUrlSigned(const std::string &_method,
					const hiaux::hashtable<std::string, std::string> &_get_params,
					std::string &_requrl) const {
	
	buildPostRequestUrlSigned(_method, _requrl);

	hiaux::hashtable<std::string, std::string>::const_iterator it = _get_params.begin();
	hiaux::hashtable<std::string, std::string>::const_iterator end = _get_params.end();
	while (it != end) {
		_requrl += "&" + it->first + "=" + it->second;
		it++;
	}
}

void HttpApiClient::buildPostRequestUrlSigned(const std::string &_method,
											std::string &_requrl) const {
	if (!m_is_auth_info_set)
		throw "HttpApiClient::buildPostRequestUrlSigned !m_is_auth_info_set\n";

	std::string ts = uint64_to_string(time(0));
	std::string sign_raw = _method + ts + m_api_key;
	unsigned char sign[21];
	sha1::calc(sign_raw.c_str(), sign_raw.size(), sign);

	char sign_hex[41];
	sha1::toHexString(sign, sign_hex);

	_requrl = m_url + "?method=" + _method + "&api_userid=" + m_api_userid + "&ts=" + ts + "&sign=" + sign_hex;
}

void HttpApiClient::buildRequestUrl(const std::string &_method,
					const hiaux::hashtable<std::string, std::string> &_get_params,
					std::string &_requrl) const {

	_requrl = m_url + "?method=" + _method + "&api_userid=" + m_api_userid;

	hiaux::hashtable<std::string, std::string>::const_iterator it = _get_params.begin();
	hiaux::hashtable<std::string, std::string>::const_iterator end = _get_params.end();
	while (it != end) {
		_requrl += "&" + it->first + "=" + it->second;
		it++;
	}
}

void HttpApiClient::call(const std::string &_method,
						const hiaux::hashtable<std::string, std::string> &_get_params,
						std::string &_resp) const {
	std::string req_url;
	buildRequestUrl(_method, _get_params, req_url);
	
	m_http_cli->callSimple(req_url, _resp);
}

void HttpApiClient::callSigned(const std::string &_method,
					const hiaux::hashtable<std::string, std::string> &_get_params,
					std::string &_resp) const {
	std::string req_url;
	buildRequestUrlSigned(_method, _get_params, req_url);

	m_http_cli->callSimple(req_url, _resp);
}

void HttpApiClient::callSignedPost(const std::string &_method,
			const hiaux::hashtable<std::string, std::string> &_params,
			std::string &_resp) const {
	
	std::string req_url;
	std::string post_data;
	buildPostRequestUrlSigned(_method, req_url);
	
	HttpApiPostData pb;
	
	hiaux::hashtable<std::string, std::string>::const_iterator it = _params.begin();
	hiaux::hashtable<std::string, std::string>::const_iterator end = _params.end();
	while (it != end) {
		HttpApiPostDataField *pb_field = pb.add_fields();
		pb_field->set_field(it->first);
		pb_field->set_value(it->second);
		it++;
	}
	
	std::string dump = pb.SerializeAsString();
	//escapeUrl(dump);
	
	m_http_cli->callPost(req_url, dump, _resp);
}
