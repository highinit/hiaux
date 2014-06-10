/* 
 * File:   HttpSrv.h
 * Author: phrk
 *
 * Created on December 28, 2013, 7:07 PM
 */

#ifndef HTTPSRV_H
#define	HTTPSRV_H

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
		std::string body;
		
		Request() { }
		Request(const std::string &_url);
		bool getField(const std::string &_key, std::string &_value);
		std::string toJson();
	};
	
	typedef boost::shared_ptr<Request> RequestPtr;
	
	class Connection
	{
	public:
		bool alive;
		bool closing;
	private:
		int m_sock;
		std::string readbf;
		http_parser m_parser;
		http_parser_settings m_parser_settings;
		
		Request cur_request;
		std::queue<RequestPtr> requests;
		
		ResponseInfoPtr m_resp_info;
		
		bool recv();
		void parseRequests();
	public:
		Connection(int sock, ResponseInfoPtr resp_info);
		~Connection();
		
		int onMessageBegin();
		int onUrl(const char *at, size_t length);
		int onStatus(const char *at, size_t length);
		int onHeadersField(const char *at, size_t length);
		int onHeadersValue(const char *at, size_t length);
		int onHeadersComplete();
		int onBody(const char *at, size_t length);
		int onMessageComplete();
		
		RequestPtr getNextRequest();
		void close();
		int getSock();
		//void send(const std::string &_mess);
		void sendResponse(const std::string &_content);
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
	
	void handler(hPoolServer::ConnectionPtr pool_conn);
	
	ConnectionPtr getHttpConn(int socket);
	ConnectionPtr getHttpConnConst(int socket);
	
	HttpSrv(TaskLauncherPtr launcher,
			const ResponseInfo &_resp_info,
			boost::function<void(HttpSrv::ConnectionPtr,
								HttpSrv::RequestPtr)> request_hdl);
	~HttpSrv();
	void start(int port);
};

typedef boost::shared_ptr<HttpSrv> HttpSrvPtr;

#endif	/* HTTPSRV_H */

