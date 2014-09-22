#ifndef _HTTP_CONNECTION_H_
#define _HTTP_CONNECTION_H_

#include "hiconfig.h"
#include <boost/shared_ptr.hpp>

#include "ServerUtils.h"
#include "ResponseInfo.h"
#include "Request.h"
#include "thirdparty/http-parser/http_parser.h"

class HttpConnection {
public:
	int sock;
	uint64_t create_ts;
	bool request_finished;
	bool recv_ok;
	
	HttpRequestPtr request;
	ResponseInfo m_resp_info;
	
	HttpConnection(int _sock, ResponseInfo _resp_info);
	~HttpConnection();
	
	void performRecv();
	bool notDead();
	
	void sendResponse(const std::string &_content);
	void setHttpStatus(int code);
	void addHeader(const std::string &_header);
	void setCookie(const std::string &_name, const std::string &_value);
	
	
	// Parsing
	http_parser m_parser;
	http_parser_settings m_parser_settings;
	
	int onMessageBegin();
	int onUrl(const char *at, size_t length);
	int onStatus(const char *at, size_t length);
	int onHeadersField(const char *at, size_t length);
	int onHeadersValue(const char *at, size_t length);
	int onHeadersComplete();
	int onBody(const char *at, size_t length);
	int onMessageComplete();
	
private:
	std::string m_cur_header_field;
	std::vector<std::string> m_headers;
	int m_http_status_code;
};

typedef boost::shared_ptr<HttpConnection> HttpConnectionPtr;

int HttpConnection_onMessageBegin(http_parser* parser);
int HttpConnection_onUrl(http_parser* parser, const char *at, size_t length);
int HttpConnection_onStatus(http_parser* parser, const char *at, size_t length);
int HttpConnection_onHeadersField(http_parser* parser, const char *at, size_t length);
int HttpConnection_onHeadersValue(http_parser* parser, const char *at, size_t length);
int HttpConnection_onHeadersComplete(http_parser* parser);
int HttpConnection_onBody(http_parser* parser, const char *at, size_t length);
int HttpConnection_onMessageComplete(http_parser* parser);

#endif
