/* 
 * File:   WebSocketSrv.h
 * Author: phrk
 *
 * Created on December 25, 2013, 8:11 PM
 */

#ifndef WEBSOCKETSRV_H
#define	WEBSOCKETSRV_H

#include "../hpoolserver.h"
#include "cwebsocket/websocket.h"
#include <tr1/unordered_map>


class WebSocketSrv
{
	class Connection
	{
	public:
		
	};
	
	
	TaskLauncherPtr m_launcher;
	hPoolServerPtr m_poolserver;
	
	std::tr1::unordered_map<int, Connection> connections;
	
	
	bool isWebSocket(const std::string &bf, std::string &key);
	
public:
	
	void handler(hPoolServer::ConnectionPtr connection);
	
	WebSocketSrv(TaskLauncherPtr launcher);
	void start(int port);
	
};

typedef boost::shared_ptr<WebSocketSrv> WebSocketSrvPtr;

#endif	/* WEBSOCKETSRV_H */

