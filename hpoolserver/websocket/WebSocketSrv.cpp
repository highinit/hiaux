#include "WebSocketSrv.h"

#include <sstream>
///#define PORT 8080
#define BUF_LEN 0x1FF
#define PACKET_DUMP

WebSocketSrv::WebSocketSrv(TaskLauncherPtr launcher):
	m_launcher(launcher)
{
	m_poolserver.reset(new hPoolServer(launcher, 
					boost::bind(&WebSocketSrv::handler, this, _1)));
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

bool WebSocketSrv::isWebSocket(const std::string &bf, std::string &key)
{
	std::vector<std::string> http_headers;
	split(bf, '\n', http_headers);
	for (int i = 0; i<http_headers.size(); i++) {
		std::cout << "header: " << http_headers[i] << std::endl;
	}
}

void WebSocketSrv::handler(hPoolServer::ConnectionPtr connection)
{
	char bf[BUF_LEN];
	int nread = ::recv(connection->m_sock, bf, BUF_LEN, 0);
	std::cout << bf << std::endl;
	std::cout << "read bytes: " << nread << std::endl;
	std::string key;
	if (isWebSocket(bf, key)) {
		
	}
	
	connection->close();
}

void WebSocketSrv::start(int port)
{
	m_poolserver->start(port);
}