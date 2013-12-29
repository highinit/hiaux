#include "HttpSrv.h"
#include "../../common/string_utils.h"

HttpSrv::Request::Request(const std::string &_url)
{
	value = _url;
}

HttpSrv::Connection::Connection(int sock):
	m_sock(sock),
	alive(true)
{
}

HttpSrv::Connection::~Connection()
{
//	std::cout << "http connection closed\n";
}

bool HttpSrv::Connection::recv()
{
	char bf[100];
	int nread = ::recv(m_sock, bf, 100, MSG_DONTWAIT);
	if (nread>0) {
		readbf.append(bf);
		return true;
	} else if (nread == 0) {
		alive  = false;
	}
	return false;
}

void HttpSrv::Connection::send(const std::string &_mess)
{
	size_t nsent = ::send(m_sock, _mess.c_str(), _mess.size(), MSG_DONTWAIT);
	if (nsent<=0)
		std::cout << "SEND ERROR!!_____________";
}

void HttpSrv::Connection::parseRequests()
{
	std::vector<std::string> lines;
	if (readbf.size()==0)
		return;
	int nextline_pos = readbf.find('\n');
	while (nextline_pos!=-1) {
		if (nextline_pos>1)
			lines.push_back(readbf.substr(0, nextline_pos));
		readbf = readbf.substr(nextline_pos+1, readbf.size()-nextline_pos-1);
		nextline_pos = readbf.find('\n');
	}
	if (lines.size()==0)
		return;
	
	for (int i = 0; i<lines.size(); i++)
		if (lines[i].substr(0,3)=="GET")
			requests.push(Request(lines[i]));

	lines.clear();
}

bool HttpSrv::Connection::getNextRequest(HttpSrv::Request &req)
{
	recv();
	parseRequests();
	if (requests.size()==0)
		return false;
	req = requests.front();
	requests.pop();
	return true;
}

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
		http_conn.reset(new Connection(socket));
		connections.insert(std::pair<int,ConnectionPtr>(socket, http_conn));
		return http_conn;
	} else
		return it->second;
}

void HttpSrv::closeHttpConn(int socket)
{
	ConnectionPtr http_conn;
	std::tr1::unordered_map<int, ConnectionPtr>::iterator it = 
							connections.find(socket); 
	if (it!=connections.end())
		connections.erase(it);
}

void HttpSrv::handler(hPoolServer::ConnectionPtr pool_conn)
{
	ConnectionPtr http_conn = getHttpConn(pool_conn->m_sock);
	
	Request req;
	bool got_req = http_conn->getNextRequest(req);

	if (!http_conn->alive) {
		closeHttpConn(pool_conn->m_sock);
		pool_conn->close();
		return;
	}

	if (got_req) {
		std::cout << "REQUEST: " << req.value << std::endl;
		std::tr1::unordered_map<std::string, std::string> values_GET;
		parseGET(req.value, values_GET);
		std::cout << "KEY0: "<< values_GET["key0"] << std::endl;
		std::cout << "KEY1: "<< values_GET["key1"] << std::endl;
	}
}

void HttpSrv::start(int port)
{
	m_poolserver->start(port);
}
