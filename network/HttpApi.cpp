#include "HttpApi.h"
#include "hiaux/crypt/sha1.h"

HttpApi::HttpApi(boost::function<std::string(const std::string&)> _buildApiError):
 m_buildApiError(_buildApiError) {
	
}

bool HttpApi::isSigned(const std::string &_method) const {
	hiaux::hashtable<std::string, int>::const_iterator it = m_signed.find(_method);
	if (it != m_signed.end())
		return true;
	else
		return false;
}

bool HttpApi::checkFields(hiaux::hashtable<std::string, std::string> &_fields, std::string &_err_mess) const {
	if (_fields.find("method") == _fields.end()) {
		_err_mess = "method not set";
		return false;
	}
	
	hiaux::hashtable<std::string, std::vector<std::string> >::const_iterator it = m_methods_args.find(_fields["method"]);
	if (it == m_methods_args.end()) {
		_err_mess = "method not found";
		return false;
	}
	
	std::vector<std::string>::const_iterator fields_it = it->second.begin();
	std::vector<std::string>::const_iterator fields_end = it->second.end();
	while (fields_it != fields_end) {
		if (_fields.find(*fields_it) == _fields.end()) {
			_err_mess = "not all method's params set";
			return false;
		}
		fields_it++;
	}
	
	if (m_signed.find(_fields["method"]) != m_signed.end()) {
		if (_fields.find("api_userid") == _fields.end() ||
			_fields.find("ts") == _fields.end() ||
			_fields.find("sign") == _fields.end()) {
				_err_mess = "not all service fields set (api_userid, ts, sign)";
				return false;
			}
		
		if (m_keys.find( _fields["api_userid"] ) == m_keys.end()) {
			_err_mess = "auth fail";
			return false;
		}
		
		std::string sign_raw = _fields["method"] + _fields["ts"] + m_keys.find( _fields["api_userid"] )->second;
		unsigned char sign[21];
		sha1::calc(sign_raw.c_str(), sign_raw.size(), sign);
		char sign_hex[41];
		sha1::toHexString(sign, sign_hex);
		
		//std::cout << "actual sign: " << sign_hex << std::endl;
		//std::cout << "got sign: " <<  _fields["sign"] << std::endl;
		
		if (_fields["sign"] != std::string(sign_hex)) {
			_err_mess = "auth fail";
			return false;
		}
	}
	return true;
}

void HttpApi::addKey(const std::string &_userid, const std::string &_key) {
	m_keys[_userid] = _key;
}

void HttpApi::addMethod(const std::string &_name,
				const std::vector<std::string> &_args_names,
				boost::function<void(hiaux::hashtable<std::string, std::string> &, std::string&)> _onreq) {
	m_methods_args[_name] = _args_names;
	m_methods_callbacks[_name] = _onreq;
}

// checks sign = concat name . [{_arg_name}] . ts; fields: method_sign, ts 
void HttpApi::addMethodSigned(const std::string &_name,
					const std::vector<std::string> &_args_names,
					boost::function<void(hiaux::hashtable<std::string, std::string> &, std::string&)> _onreq,
					uint64_t _max_ts_range) {
	m_methods_args[_name] = _args_names;
	m_methods_callbacks[_name] = _onreq;
	m_signed[_name] = true;
}

void HttpApi::handle(HttpSrv::ConnectionPtr _conn, HttpSrv::RequestPtr _req) {

	std::string err_mess;

	if (!checkFields (_req->values_GET, err_mess)) {
		_conn->sendResponse( m_buildApiError( err_mess ) );
	} else {
		std::string resp;
		m_methods_callbacks[ _req->values_GET["method"] ] ( _req->values_GET , resp);
		_conn->sendResponse(resp);
	}
	_conn->close();
}
