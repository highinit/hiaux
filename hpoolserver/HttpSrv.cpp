#include "HttpSrv.h"

HttpSrv::ResponseInfo::ResponseInfo(const std::string &_content_type,
					const std::string &_server_name):
		content_type(_content_type),
		server_name(_server_name)
{
}

HttpSrv::Request::Request(const std::string &_url)
{
	std::cout << "Request: " << _url << std::endl; 
	parseGET(_url, values_GET);
}

int HttpSrv_http_data_cb (http_parser* parser, const char *at, size_t length)
{
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	conn->onDataHttp(at, length);
}

int HttpSrv_http_cb (http_parser* parser)
{
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	conn->onEventHttp();
}

HttpSrv::Connection::Connection(int sock, ResponseInfoPtr resp_info):
		m_sock(sock),
		alive(true),
		closing(false),
		m_resp_info(resp_info)
{
	http_parser_init(&m_parser, HTTP_REQUEST);
	m_parser.data = (void*)this;
	m_parser_settings.
}

HttpSrv::Connection::~Connection()
{
	//std::cout << "http connection closed\n";
	delete parser;
}

int HttpSrv::Connection::onDataHttp (const char *at, size_t length)
{

}

int HttpSrv::Connection::onEventHttp ()
{
		
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

void HttpSrv::Connection::sendResponse(const std::string &_content)
{
	//Sat, 28 Dec 2013 18:33:30 GMT
	char content_len_c[50];
	sprintf(content_len_c, "%d", _content.size());
	std::string content_len(content_len_c);
	
	//char time_c[50];
	//sprintf(time_c, "%d", asctime(0));
	
	char time_c[50];
	sprintf(time_c, "%d", time(0));
	
	std::string response = "HTTP/1.1 200 OK\r\n"
						"Content-Type: "+m_resp_info->content_type+"\r\n"
						"Date: "+time_c+"\r\n"
						"Server: "+m_resp_info->server_name+"\r\n"
						"Connection: keep-alive\r\n"
						"Transfer-Encoding: none\r\n"
						"Access-Control-Allow-Origin: *\r\n"
						"Content-Length: "+content_len+"\n\n"+_content+"\r\n";
	size_t nsent = ::send(m_sock, response.c_str(), response.size(), MSG_DONTWAIT);
	if (nsent<=0)
		std::cout << "HttpSrv::Connection::sendResponse SEND ERROR!!_____________"
				<< nsent << std::endl;
}

/*
void HttpSrv::Connection::send(const std::string &_mess)
{
	size_t nsent = ::send(m_sock, _mess.c_str(), _mess.size(), MSG_DONTWAIT);
	if (nsent<=0)
		std::cout << "SEND ERROR!!_____________";
}*/

void HttpSrv::Connection::close()
{
	closing = true;
}

void HttpSrv::Connection::parseRequests()
{
	
	return;
	std::vector<std::string> lines;
	if (readbf.size()==0)
		return;
	std::cout << "ReadBF: " << readbf << std::endl;
	int nextline_pos = readbf.find('\n');
	while (nextline_pos!=-1) {
		if (nextline_pos>1)
			lines.push_back(readbf.substr(0, nextline_pos));
		readbf = readbf.substr(nextline_pos+1, readbf.size()-nextline_pos-1);
		nextline_pos = readbf.find('\n');
	}
	if (lines.size()==0)
		return;
	
	for (int i = 0; i<lines.size(); i++) {
		std::cout << "Line: " << lines[i] << std::endl; 
		if (lines[i].substr(0,3)=="GET")
			requests.push(RequestPtr(new Request(lines[i])));
	}
	
	lines.clear();
}

HttpSrv::RequestPtr HttpSrv::Connection::getNextRequest()
{
	RequestPtr req;
	recv();
	parseRequests();
	
	if (requests.size()==0) 
		return req;
	req = requests.front();
	requests.pop();
	return req;
}

HttpSrv::HttpSrv(TaskLauncherPtr launcher,
			const HttpSrv::ResponseInfo &resp_info,
			boost::function<void(HttpSrv::ConnectionPtr,
								HttpSrv::RequestPtr)> request_hdl):
		m_launcher(launcher),
		m_resp_info(new ResponseInfo(resp_info)),
		m_request_hdl(request_hdl)
{
	m_poolserver.reset(new hPoolServer(launcher, 
					boost::bind(&HttpSrv::handler, this, _1)));
}

HttpSrv::ConnectionPtr HttpSrv::getHttpConn(int socket)
{
	hLockTicketPtr ticket = m_connections_lock.lock();
	ConnectionPtr http_conn;
	hiaux::hashtable<int, ConnectionPtr>::iterator it = 
							connections.find(socket); 
	if (it==connections.end()) {
		http_conn.reset(new Connection(socket, m_resp_info));
		connections.insert(std::pair<int,ConnectionPtr>(socket, http_conn));
		return http_conn;
	} else
		return it->second;
}

void HttpSrv::closeHttpConn(int socket)
{
	hLockTicketPtr ticket = m_connections_lock.lock();
	ConnectionPtr http_conn;
	hiaux::hashtable<int, ConnectionPtr>::iterator it = 
							connections.find(socket); 
	if (it!=connections.end())
		connections.erase(it);
}

void HttpSrv::handler(hPoolServer::ConnectionPtr pool_conn)
{
	ConnectionPtr http_conn = getHttpConn(pool_conn->m_sock);
	
	RequestPtr req = http_conn->getNextRequest();

	if (!http_conn->alive) {
		closeHttpConn(pool_conn->m_sock);
		pool_conn->close();
		return;
	}

	if (req) {
		m_request_hdl(http_conn, req);
		if (!http_conn->alive || http_conn->closing) {
			closeHttpConn(pool_conn->m_sock);
			pool_conn->close();
		}
	}
}

void HttpSrv::start(int port)
{
	m_poolserver->start(port);
}
