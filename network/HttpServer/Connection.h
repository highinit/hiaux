#ifndef _HTTP_CONNECTION_H_
#define _HTTP_CONNECTION_H_

#include "hiconfig.h"
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <queue>

#include "ServerUtils.h"
#include "ResponseInfo.h"
#include "Request.h"
#include "Response.h"
#include "thirdparty/http-parser/http_parser.h"

#include "CustomParser.h"

#include <sys/types.h>
#include <sys/socket.h>

class HttpConnection : public boost::noncopyable {
public:
	int sock;
	uint64_t create_ts;
	uint64_t last_activity_ts;
	bool request_finished;
	bool alive;
	bool ever_sent;
	bool keepalive;
	bool custom_protocol;
	bool waiting_last_handling;
	
	std::string custom_protocol_id;
	
	std::queue<HttpRequestPtr> http_requests;
	std::queue<CustomRequestPtr> custom_requests;
	
	
	HttpConnection(int _sock, ResponseInfo _resp_info, const boost::function<void(int, const HttpResponse &)> &_on_send_response,
					const boost::function<void(int, const std::string &)> &_on_send_custom_response,
					const boost::function<CustomParserPtr(const std::string &_protocol, const HttpRequestPtr &_req, std::string &_handshake)> &_getCustomParser);
					
	~HttpConnection();
	
	bool checkUpgrade(HttpRequestPtr request);
	
	void performRecv();
	bool notDead();
	
	void sendResponse(const HttpResponse &_resp);
	
	void sendCustomResponse(const std::string &_resp);
	
	void addResponse(const HttpResponse &_resp);
	void addCustomResponse(const std::string &_resp);
	bool performSend();
	
	//void sendResponse(const std::string &_content);
	//void sendResponse();
	
	void setHttpStatus(int code);
	void addHeader(const std::string &_header);
	void setCookie(const std::string &_name, const std::string &_value);
	
	
	int onMessageBegin();
	int onUrl(const char *at, size_t length);
	int onStatus(const char *at, size_t length);
	int onHeadersField(const char *at, size_t length);
	int onHeadersValue(const char *at, size_t length);
	int onHeadersComplete();
	int onBody(const char *at, size_t length);
	int onMessageComplete();
	
private:
	
	void resetHttpParser();
	void renderResponse(const HttpResponse &_resp, std::string &_response);
	
	boost::function<void(int, const HttpResponse &)> m_on_send_response;
	boost::function<void(int, const std::string &)> m_on_send_custom_response;
	boost::function<CustomParserPtr(const std::string &_protocol, const HttpRequestPtr &_req, std::string &)> m_getCustomParser;
	
	HttpRequestPtr m_cur_http_request;
	CustomRequestPtr m_cur_custom_request;
	
	ResponseInfo m_resp_info;
	
	std::queue<std::string> m_resps;
	std::string m_send_buffer;
	
	std::string m_cur_header_field;
	std::vector<std::string> m_headers;
	int m_http_status_code;
	
	http_parser m_parser;
	http_parser_settings m_parser_settings;
	
	CustomParserPtr m_custom_parser;
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
