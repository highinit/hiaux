/* 
 * File:   HttpSrv.h
 * Author: phrk
 *
 * Created on December 28, 2013, 7:07 PM
 */

#ifndef _HTTPSRV_H_
#define	_HTTPSRV_H_

#include "hiconfig.h"
#include <cstdio>
#include "hpoolserver.h"
#include "hiaux/structs/hashtable.h"
#include "hiaux/strings/string_utils.h"

#include "thirdparty/http-parser/http_parser.h"

#include <jansson.h>

#include <sys/types.h>
#include <sys/socket.h>

class HttpSrv
{
public:
	
	class ResponseInfo
	{
	public:
		std::string content_type;
		std::string server_name;
		ResponseInfo(const std::string &_content_type,
					const std::string &_server_name);
	};
	
	typedef boost::shared_ptr<ResponseInfo> ResponseInfoPtr;
	
	class Request
	{
	public:
		//std::string value;
		std::string url;
		std::string path;
		hiaux::hashtable<std::string, std::string> values_GET;
		hiaux::hashtable<std::string, std::string> cookies;
		std::string body;
		
		Request() { }
		Request(const std::string &_url);
		bool getField(const std::string &_key, std::string &_value);
		bool getCookie(const std::string &_name, std::string &_value);
		std::string toJson();
	};
	
	typedef boost::shared_ptr<Request> RequestPtr;
	
	class Connection {
	public:
		bool alive;
		bool closing;
	private:
		int m_sock;
		std::string m_readbf;
		std::string m_sendbf;
		http_parser m_parser;
		http_parser_settings m_parser_settings;
		
		std::string m_cur_header_field;
		
		Request m_cur_request;
		std::queue<RequestPtr> m_requests;
		
		ResponseInfoPtr m_resp_info;
		
		std::vector<std::string> m_headers;
		
		int m_http_status_code;
		
		boost::function<void(int, HttpSrv::RequestPtr)> m_onRequest;
		
		void parseRequests();
	public:
		Connection(int _sock,
					ResponseInfoPtr _resp_info,
					boost::function<void(int, HttpSrv::RequestPtr)> _onRequest);
		~Connection();
		
		int onMessageBegin();
		int onUrl(const char *at, size_t length);
		int onStatus(const char *at, size_t length);
		int onHeadersField(const char *at, size_t length);
		int onHeadersValue(const char *at, size_t length);
		int onHeadersComplete();
		int onBody(const char *at, size_t length);
		int onMessageComplete();
		
		//RequestPtr getNextRequest();
		void close();
		int getSock();
		//void send(const std::string &_mess);
		void setHttpStatus(int code);
		void addHeader(const std::string &_header);
		void setCookie(const std::string &_name, const std::string &_value);
		void sendResponse(const std::string &_content);
		
		void performRecv();
		void performSend();
	};
	
	typedef boost::shared_ptr<Connection> ConnectionPtr;

private:
	TaskLauncherPtr m_launcher;
	hPoolServerPtr m_poolserver;
	
	// socket / connection
	hiaux::hashtable<int, ConnectionPtr> connections;
	hAutoLock m_connections_lock;
	
	boost::function<void(HttpSrv::ConnectionPtr, HttpSrv::RequestPtr)> m_request_hdl;
	
	ResponseInfoPtr m_resp_info;
	
	void closeHttpConn(int socket);
public:
	
//	void handler(hPoolServer::ConnectionPtr pool_conn);
	
	void onRead(hPoolServer::ConnectionPtr _pool_conn);
	void onWrite(hPoolServer::ConnectionPtr _pool_conn);
	void onError(hPoolServer::ConnectionPtr _pool_conn);
	
	void onRequest(int _fd, HttpSrv::RequestPtr _req);
	
	ConnectionPtr getHttpConn(int socket);
	ConnectionPtr getHttpConnConst(int socket);
	
	void checkConnClose(hPoolServer::ConnectionPtr _pool_conn, ConnectionPtr _conn);
	
	HttpSrv(TaskLauncherPtr launcher,
			const ResponseInfo &_resp_info,
			boost::function<void(HttpSrv::ConnectionPtr,
								HttpSrv::RequestPtr)> request_hdl);
	~HttpSrv();
	void start(int port);
};

typedef boost::shared_ptr<HttpSrv> HttpSrvPtr;

int HttpSrv_onMessageBegin(http_parser* parser);
int HttpSrv_onUrl(http_parser* parser, const char *at, size_t length);
int HttpSrv_onStatus(http_parser* parser, const char *at, size_t length);
int HttpSrv_onHeadersField(http_parser* parser, const char *at, size_t length);
int HttpSrv_onHeadersValue(http_parser* parser, const char *at, size_t length);
int HttpSrv_onHeadersComplete(http_parser* parser);
int HttpSrv_onBody(http_parser* parser, const char *at, size_t length);
int HttpSrv_onMessageComplete(http_parser* parser);

#endif	/* _HTTPSRV_H_ */

