#ifndef _HTTPAPI_H_
#define _HTTPAPI_H_

#include "hiconfig.h"
#include "../HttpServer/HttpServer.h"

#include "HttpApiPostData.pb.h"

#include "hiaux/crypt/base64.h"

class HttpApi {
	hiaux::hashtable<std::string, std::vector<std::string> > m_methods_args;
	hiaux::hashtable<std::string, int> m_signed;
	hiaux::hashtable<std::string, int> m_async;	
	hiaux::hashtable<std::string, boost::function<void(hiaux::hashtable<std::string, std::string> &, std::string&)> > m_methods_callbacks;
	
	hiaux::hashtable<std::string, boost::function<void(hiaux::hashtable<std::string, std::string> &, 
														boost::function <void(const std::string&)> )> > m_methods_callbacks_async;
	
	
	bool isSigned(const std::string &_method) const;
	bool isAsync(const std::string &_method) const;
	bool checkFields(hiaux::hashtable<std::string, std::string> &_fields, std::string &_err_mess) const;
	
	hiaux::hashtable<std::string, std::string> m_keys;
	
	boost::function<std::string(const std::string&)> m_buildApiError;
	
	void mergePostParams(hiaux::hashtable<std::string, std::string> &_params, const std::string &_body);
	
	void onAsyncCallDone(const std::string &, HttpConnectionPtr _conn);
	
public:
	
	HttpApi(boost::function<std::string(const std::string &)> _buildError);
	
	void addUser(const std::string &_userid, const std::string &_key);
	
	void addMethod(const std::string &_name,
					const std::vector<std::string> &_args_names,
					boost::function<void(hiaux::hashtable<std::string, std::string> &, std::string&)> _onreq);
	
	// checks sign = concat name . [{_arg_name}] . ts; fields: method_sign, ts 
	void addMethodSigned(const std::string &_name,
						const std::vector<std::string> &_args_names,
						boost::function<void(hiaux::hashtable<std::string, std::string> &, std::string&)> _onreq,
						uint64_t _max_ts_range = 15);

	void addMethodSignedAsync(const std::string &_name,
						const std::vector<std::string> &_args_names,
						boost::function<void(hiaux::hashtable<std::string, std::string> &,
												boost::function <void(const std::string&)>  )> _onreq,
						uint64_t _max_ts_range = 15);
	
	void handle(HttpConnectionPtr _conn, HttpRequestPtr _req);
};

typedef boost::shared_ptr<HttpApi> HttpApiPtr;

#endif
