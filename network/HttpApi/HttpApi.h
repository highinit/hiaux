#ifndef _HTTPAPI_H_
#define _HTTPAPI_H_

#include "hiconfig.h"
#include "../HttpServer/HttpServer.h"
#include <sstream>
#include "HttpApiPostData.pb.h"

#include "hiaux/crypt/base64.h"
#include "hiaux/crypt/sha1.h"

#include "hiaux/network/HttpApi/BinServer/Request.h"

#include <map>

namespace hiaux {

template <class ConnectionDataT = int>
class HttpApi {
	std::map<std::string, std::vector<std::string> > m_methods_args;
	std::map<std::string, int> m_signed;
	std::map<std::string, int> m_async;	
	std::map<std::string, boost::function<void(hiaux::HttpConnectionPtr<ConnectionDataT>, std::map<std::string, std::string> &, std::string&)> > m_methods_callbacks;
	
	std::map<std::string, boost::function<void(std::map<std::string, std::string> &, 
														boost::function <void(const std::string&)> )> > m_methods_callbacks_async;
	
	
	bool isSigned(const std::string &_method) const;
	bool isAsync(const std::string &_method) const;
	bool checkFields(const std::map<std::string, std::string> &_fields, std::string &_err_mess) const;
	
	std::map<std::string, std::string> m_keys;
	
	boost::function<std::string(const std::string&)> m_buildApiError;
	
	void mergePostParams(std::map<std::string, std::string> &_params, const std::string &_body);
	
	void onAsyncCallDone(const std::string &, HttpConnectionPtr<ConnectionDataT> _conn);
	
	void handleRequest(HttpConnectionPtr<ConnectionDataT> _conn, std::map<std::string, std::string> &_params, bool _bin);
	
public:
	
	HttpApi(boost::function<std::string(const std::string &)> _buildError);
	
	void setUserToken(const std::string &_userid, const std::string &_token);
	
	void addMethod(const std::string &_name,
					const std::vector<std::string> &_args_names,
					boost::function<void(hiaux::HttpConnectionPtr<ConnectionDataT>, std::map<std::string, std::string> &, std::string&)> _onreq);
	
	// checks sign = concat name . [{_arg_name}] . ts; fields: method_sign, ts 
	void addMethodSigned(const std::string &_name,
						const std::vector<std::string> &_args_names,
						boost::function<void(hiaux::HttpConnectionPtr<ConnectionDataT>, std::map<std::string, std::string> &, std::string&)> _onreq,
						uint64_t _max_ts_range = 15);

	void addMethodSignedAsync(const std::string &_name,
						const std::vector<std::string> &_args_names,
						boost::function<void(std::map<std::string, std::string> &,
												boost::function <void(const std::string&)>  )> _onreq,
						uint64_t _max_ts_range = 15);
	
	void handle(HttpConnectionPtr<ConnectionDataT> _conn, HttpRequestPtr _req);
	void handleBinary(HttpConnectionPtr<ConnectionDataT> _conn, CustomRequestPtr _req);
};

template <class ConnectionDataT = int>
using HttpApiPtr = boost::shared_ptr<HttpApi <ConnectionDataT> >;

#include "HttpApi.impl"

}

#endif
