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

bool HttpApi::isAsync(const std::string &_method) const {
	hiaux::hashtable<std::string, int>::const_iterator it = m_async.find(_method);
	if (it != m_async.end())
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

void HttpApi::addUser(const std::string &_userid, const std::string &_key) {
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

void HttpApi::addMethodSignedAsync(const std::string &_name,
					const std::vector<std::string> &_args_names,
					boost::function<void(hiaux::hashtable<std::string, std::string> &,
											boost::function <void(const std::string&)>  )> _onreq,
					uint64_t _max_ts_range) {
	
	m_methods_args[_name] = _args_names;
	m_methods_callbacks_async[_name] = _onreq;
	m_signed[_name] = true;
	m_async[_name] = true;				
}

void HttpApi::mergePostParams(hiaux::hashtable<std::string, std::string> &_params, const std::string &_body) {
	
//	std::cout << "mergePostParams: " << _body << std::endl;
	try {
		HttpApiPostData pb;
		std::string body = base64_decode(_body);
		//unescapeUrl(body);
		pb.ParseFromString(body);
//		std::cout << "pb.fields_size(): " << pb.fields_size() << std::endl;
		for (int i = 0; i<pb.fields_size(); i++) {
			HttpApiPostDataField field = pb.fields(i);
			_params[field.field()] = field.value();
//			std::cout << "HttpApi::mergePostParams " <<  field.field() << "/" << field.value() << std::endl;
		}
	} catch (...) {
		std::cout << "HttpApi::mergePostParams HttpApiPostData protobuf parsing exception\n";
	}
}

void HttpApi::onAsyncCallDone(const std::string &_resp, HttpSrv::ConnectionPtr _conn) {
	
	_conn->sendResponse(_resp);
	_conn->close();
}

void HttpApi::handle(HttpSrv::ConnectionPtr _conn, HttpSrv::RequestPtr _req) {

	hiaux::hashtable<std::string, std::string> params = _req->values_GET;
	mergePostParams(params, _req->body);
	
	std::string err_mess;

	if (!checkFields (params, err_mess)) {
		_conn->sendResponse( m_buildApiError( err_mess ) );
		_conn->close();
	} else {
		std::string resp;
		
		if (isAsync(params["method"])) {
			
			hiaux::hashtable<std::string, boost::function<void(hiaux::hashtable<std::string, std::string> &, 
																boost::function <void(const std::string&)> )> >::iterator it =
																	 m_methods_callbacks_async.find(params["method"]);
			
			if (it == m_methods_callbacks_async.end()) {
				_conn->sendResponse("No such method");
				_conn->close();
			}
			else {
				it->second(params, boost::bind(&HttpApi::onAsyncCallDone, this, _1, _conn));
			}
			
		} else {
		
			hiaux::hashtable<std::string, boost::function<void(hiaux::hashtable<std::string, std::string> &, std::string&)> >::iterator it = 
				m_methods_callbacks.find(params["method"]);
			if (it == m_methods_callbacks.end()) {
				_conn->sendResponse("No such method");
			}
			else {
				it->second(params, resp);
				_conn->sendResponse(resp);
			}
			_conn->close();
		}
	}
}
