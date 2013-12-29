/* 
 * File:   HttpSrv.h
 * Author: phrk
 *
 * Created on December 28, 2013, 7:07 PM
 */

#ifndef HTTPSRV_H
#define	HTTPSRV_H

#include "../hpoolserver.h"
#include <tr1/unordered_map>

#include "http_parser.h"

class HttpSrv
{
	class Request
	{
	public:
		std::string value;
		
		Request() { }
		Request(const std::string &_url);
		bool getField(const std::string name, std::string &value);
	};
	
	class Connection
	{
	public:
		/*enum State
		{
			PARSING_REQUEST,
			SENDING_RESPONSE
		};
		State state;
		 */ 
		bool alive;
	private:
		int m_sock;
		std::string readbf;
		std::string sendbf;
		std::queue<Request> requests;
		
		bool recv();
		void parseRequests();
	public:
		Connection(int sock);
		~Connection();
		bool getNextRequest(Request &req);
		void send(const std::string &_mess);
	};
	
	typedef boost::shared_ptr<Connection> ConnectionPtr;
	
	TaskLauncherPtr m_launcher;
	hPoolServerPtr m_poolserver;
	
	// socket / connection
	std::tr1::unordered_map<int, ConnectionPtr> connections;
	
	ConnectionPtr getHttpConn(int socket);
	void closeHttpConn(int socket);
public:
	
	void handler(hPoolServer::ConnectionPtr pool_conn);
	
	HttpSrv(TaskLauncherPtr launcher);
	void start(int port);
};

typedef boost::shared_ptr<HttpSrv> HttpSrvPtr;

#endif	/* HTTPSRV_H */

