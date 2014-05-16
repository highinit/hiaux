#include "HttpApi.h"
#include "hiaux/crypt/sha1.h"

HttpApi::HttpApi() {
	
}

bool HttpApi::isSigned(const std::string &_method) const {
	hiaux::hashtable<std::string, int>::const_iterator it = m_signed.find(_method);
	if (it != m_signed.end())
		return true;
	else
		return false;
}

bool HttpApi::checkFields(hiaux::hashtable<std::string, std::string> &_fields) const {
	if (_fields.find("method") == _fields.end())
		return false;
	
	hiaux::hashtable<std::string, std::vector<std::string> >::const_iterator it = m_methods.find(_fields["method"]);
	if (it == m_methods.end())
		return false;
	
	std::vector<std::string>::const_iterator fields_it = it->second.begin();
	std::vector<std::string>::const_iterator fields_end = it->second.end();
	while (fields_it != fields_end) {
		if (_fields.find(*fields_it) == _fields.end())
			return false;
		fields_it++;
	}
	
	if (m_signed.find(_fields["method"]) != m_signed.end()) {
		if (m_signed.find(_fields["api_userid"]) == m_signed.end() ||
			m_signed.find(_fields["ts"]) == m_signed.end() ||
			m_signed.find(_fields["sign"]) == m_signed.end())
				return false;
		
		if (m_keys.find( _fields["api_userid"] ) == m_keys.end())
			return false;
		
		std::string sign_raw = _fields["method"] + _fields["api_userid"] + _fields["ts"] + m_keys.find(_fields.find("api_userid")->second)->second;
		unsigned char sign[21];
		sha1::calc(sign_raw.c_str(), sign_raw.size(), sign);
		if (_fields["sign"] != std::string( (char*) sign))
			return false;
	}
	return true;
}

void HttpApi::addKey(const std::string &_userid, const std::string &_key) {
	m_keys[_userid] = _key;
}

void HttpApi::addMethod(const std::string &_name,
				const std::vector<std::string> &_args_names,
				boost::function<void(hiaux::hashtable<std::string, std::string> &, std::string&)> _onreq) {
	m_methods[_name] = _args_names;
	m_onreq = _onreq;
}

// checks sign = concat name . [{_arg_name}] . ts; fields: method_sign, ts 
void HttpApi::addMethodSigned(const std::string &_name,
					const std::vector<std::string> &_args_names,
					boost::function<void(hiaux::hashtable<std::string, std::string> &, std::string&)> _onreq,
					uint64_t _max_ts_range) {
	m_methods[_name] = _args_names;
	m_onreq = _onreq;
	m_signed[_name] = true;
}

void HttpApi::handle(HttpSrv::ConnectionPtr _conn, HttpSrv::RequestPtr _req) {
	if (!checkFields (_req->values_GET)) {
		_conn->sendResponse("HttpApi: request error");
		
	}
	_conn->close();
}
