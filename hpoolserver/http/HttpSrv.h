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
	class Connection
	{
		http_parser *m_parser;
		http_parser_settings *m_parser_settings;
	public:
		
		enum State
		{
			PARSING_REQUEST,
			SENDING_RESPONSE
		};
		
		State state;
		std::string readbf;
		std::string sendbf;
		
		Connection()
		{
			m_parser_settings = new http_parser_settings;
			http_parser_init(m_parser, HTTP_REQUEST);
			state = PARSING_REQUEST;
		}
		
		bool getField(const std::string name, std::string &value)
		{
			
		}
	};
	
	typedef boost::shared_ptr<Connection> ConnectionPtr;
	
	TaskLauncherPtr m_launcher;
	hPoolServerPtr m_poolserver;
	
	// socket / connection
	std::tr1::unordered_map<int, ConnectionPtr> connections;
	
	ConnectionPtr getHttpConn(int socket);
public:
	
	void handler(hPoolServer::ConnectionPtr pool_conn);
	
	HttpSrv(TaskLauncherPtr launcher);
	void start(int port);
};

typedef boost::shared_ptr<HttpSrv> HttpSrvPtr;

#endif	/* HTTPSRV_H */

