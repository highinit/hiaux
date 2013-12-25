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
		//std::cout << "header: " << http_headers[i] << std::endl;
		std::vector<std::string> keyvalue;
		split(http_headers[i], ':', keyvalue);
		if (keyvalue.size()==2) {
			//std::cout << "k: " << keyvalue[0];
			//std::cout << " v: " << keyvalue[1] << std::endl;
			if (keyvalue[0] == "Sec-WebSocket-Key") {
				key = keyvalue[1].substr(1, keyvalue[1].size()-1);
				return true;
			}
		}
	}
	return false;
}

void WebSocketSrv::handler(hPoolServer::ConnectionPtr connection)
{
	ConnectionPtr web_conn;
	std::tr1::unordered_map<int, ConnectionPtr>::iterator it = 
							connections.find(connection->m_sock); 
	if (it==connections.end()) {
		web_conn.reset(new Connection());
		connections.insert(std::pair<int,ConnectionPtr>(connection->m_sock, web_conn));
	} else
		web_conn = it->second;
	
	if (web_conn->state == Connection::READING_HEADERS) {
		char bf[BUF_LEN];
		int nread = ::recv(connection->m_sock, bf, BUF_LEN, 0);
		//std::cout << bf << std::endl;
		//std::cout << "read bytes: " << nread << std::endl;
		web_conn->readbf.append(bf);
		std::string key;
		if (isWebSocket(web_conn->readbf, key)) {
			std::cout << "Reading headers done key:" << key << "\n";
			
			web_conn->state = Connection::READING_FRAME;
		}
	}
	//connection->close();
}

void WebSocketSrv::start(int port)
{
	m_poolserver->start(port);
}
