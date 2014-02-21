/* 
 * File:   HttpSrv.h
 * Author: phrk
 *
 * Created on December 28, 2013, 7:07 PM
 */

#ifndef HTTPSRV_H
#define	HTTPSRV_H

#include "hiconfig.h"

#include "hpoolserver.h"
#include "hiaux/structs/hashtable.h"
#include "hiaux/strings/string_utils.h"

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
		std::string value;
		hiaux::hashtable<std::string, std::string> values_GET;
		
		Request() { }
		Request(const std::string &_url);
		bool getField(const std::string name, std::string &value);
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
		std::string sendbf;
		std::queue<RequestPtr> requests;
		
		ResponseInfoPtr m_resp_info;
		
		bool recv();
		void parseRequests();
	public:
		Connection(int sock, ResponseInfoPtr resp_info);
		~Connection();
		RequestPtr getNextRequest();
		void close();
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
	
	ConnectionPtr getHttpConn(int socket);
	void closeHttpConn(int socket);
public:
	
	void handler(hPoolServer::ConnectionPtr pool_conn);
	
	HttpSrv(TaskLauncherPtr launcher,
			const ResponseInfo &_resp_info,
			boost::function<void(HttpSrv::ConnectionPtr,
								HttpSrv::RequestPtr)> request_hdl);
	void start(int port);
};

typedef boost::shared_ptr<HttpSrv> HttpSrvPtr;

#endif	/* HTTPSRV_H */

