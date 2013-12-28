#include "HttpSrv.h"

HttpSrv::HttpSrv(TaskLauncherPtr launcher):
	m_launcher(launcher)
{
	m_poolserver.reset(new hPoolServer(launcher, 
					boost::bind(&HttpSrv::handler, this, _1)));
}

HttpSrv::ConnectionPtr HttpSrv::getHttpConn(int socket)
{
	ConnectionPtr http_conn;
	std::tr1::unordered_map<int, ConnectionPtr>::iterator it = 
							connections.find(socket); 
	if (it==connections.end()) {
		http_conn.reset(new Connection());
		connections.insert(std::pair<int,ConnectionPtr>(socket, http_conn));
		return http_conn;
	} else
		return it->second;
}

void HttpSrv::handler(hPoolServer::ConnectionPtr pool_conn)
{
	ConnectionPtr http_conn = getHttpConn(pool_conn->m_sock);
	if (http_conn->state == Connection::PARSING_REQUEST)
	{
		#define BUF_LEN 200
		char bf[BUF_LEN];
		::recv(pool_conn->m_sock, bf, BUF_LEN, 0);
		http_conn->readbf.append(bf);
		if (strlen(bf)!=0)
		std::cout << http_conn->readbf << std::endl;
		
		
		//http_conn->state = Connection::SENDING_RESPONSE;
	}
	
}

void HttpSrv::start(int port)
{
	m_poolserver->start(port);
}
