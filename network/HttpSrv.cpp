#include <queue>

#include "HttpSrv.h"

HttpSrv::ResponseInfo::ResponseInfo(const std::string &_content_type,
					const std::string &_server_name):
		content_type(_content_type),
		server_name(_server_name)
{
}

HttpSrv::Request::Request(const std::string &_url)
{
	//std::cout << "Request: " << _url << std::endl; 
	parseGET(_url, values_GET);
}

bool HttpSrv::Request::getField(const std::string &_key, std::string &_value) {
	hiaux::hashtable<std::string, std::string>::iterator it = values_GET.find(_key);
	
	if (it != values_GET.end()) {
		_value = it->second;
		return true;
	} else
		return false;
}

int HttpSrv_onMessageBegin(http_parser* parser) {
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	return conn->onMessageBegin();
}

int HttpSrv_onUrl(http_parser* parser, const char *at, size_t length) {
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	return conn->onUrl(at, length);
}

int HttpSrv_onStatus(http_parser* parser, const char *at, size_t length) {
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	return conn->onStatus(at, length);
}

int HttpSrv_onHeadersField(http_parser* parser, const char *at, size_t length) {
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	return conn->onHeadersField(at, length);
}

int HttpSrv_onHeadersValue(http_parser* parser, const char *at, size_t length) {
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	return conn->onHeadersValue(at, length);
}

int HttpSrv_onHeadersComplete(http_parser* parser) {
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	return conn->onHeadersComplete();
}

int HttpSrv_onBody(http_parser* parser, const char *at, size_t length) {
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	return conn->onBody(at, length);	
}

int HttpSrv_onMessageComplete(http_parser* parser) {
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	return conn->onMessageComplete();
}

int HttpSrv::Connection::onMessageBegin() {
	//std::cout << "HttpSrv::Connection::onMessageBegin" << std::endl;
	return 0;
}

int HttpSrv::Connection::onUrl(const char *at, size_t length) {
	//std::cout << "HttpSrv::Connection::onUrl" << at << std::endl;
	cur_request.url = std::string(at);
	cur_request.url = cur_request.url.substr(0, cur_request.url.find(' '));
	//fix_utf8_string(cur_request.url);
	cur_request.path = getUrlPath(cur_request.url);
	parseGET(cur_request.url, cur_request.values_GET);
	return 0;
}

int HttpSrv::Connection::onStatus(const char *at, size_t length) {
	//std::cout << "HttpSrv::Connection::onStatus" << at << std::endl;
	return 0;
}

int HttpSrv::Connection::onHeadersField(const char *at, size_t length) {
	//std::cout << "HttpSrv::Connection::onHeadersField" << at << std::endl;
	return 0;
}

int HttpSrv::Connection::onHeadersValue(const char *at, size_t length) {
	//std::cout << "HttpSrv::Connection::onHeadersValue" << at << std::endl;
	return 0;
}

int HttpSrv::Connection::onHeadersComplete() {
	//std::cout << "HttpSrv::Connection::onHeadersComplete" << std::endl;
	return 0;
}

int HttpSrv::Connection::onBody(const char *at, size_t length) {
	//std::cout << "HttpSrv::Connection::onBody" << at << std::endl;
	cur_request.body = std::string(at);
	fix_utf8_string(cur_request.body);
	return 0;
}

int HttpSrv::Connection::onMessageComplete() {
	//std::cout << "HttpSrv::Connection::onMessageComplete" << std::endl;
	requests.push( RequestPtr(new Request( cur_request ) ) );
	return 0;
}

HttpSrv::Connection::Connection(int sock, ResponseInfoPtr resp_info):
		m_sock(sock),
		alive(true),
		closing(false),
		m_resp_info(resp_info)
{
	http_parser_init(&m_parser, HTTP_REQUEST);
	m_parser.data = (void*)this;
	m_parser_settings.on_message_begin = HttpSrv_onMessageBegin;
	m_parser_settings.on_url = &HttpSrv_onUrl;
	m_parser_settings.on_status = &HttpSrv_onStatus;
	m_parser_settings.on_header_field = &HttpSrv_onHeadersField;
	m_parser_settings.on_header_value = &HttpSrv_onHeadersValue;
	m_parser_settings.on_headers_complete = &HttpSrv_onHeadersComplete;
	m_parser_settings.on_body = &HttpSrv_onBody;
	m_parser_settings.on_message_complete = &HttpSrv_onMessageComplete;
}


HttpSrv::Connection::~Connection()
{
	//std::cout << "http connection closed\n";
}

bool HttpSrv::Connection::recv()
{
	char bf[1024];
	int nread = ::recv(m_sock, bf, 1024, MSG_DONTWAIT);
	bool read = false;
	while (nread > 0) {
		read = true;
		std::string add (bf);
		//unescapeUrl(add);
		readbf.append( add );
		nread = ::recv(m_sock, bf, 1024, MSG_DONTWAIT);
	}
	return read;
}

void HttpSrv::Connection::sendResponse(const std::string &_content)
{
	//Sat, 28 Dec 2013 18:33:30 GMT
	char content_len_c[50];
	sprintf(content_len_c, "%d", (int)_content.size());
	std::string content_len(content_len_c);
	
	//char time_c[50];
	//sprintf(time_c, "%d", asctime(0));
	
	char time_c[50];
	sprintf(time_c, "%d", (int)time(0));
	
	std::string response = "HTTP/1.1 200 OK\r\n"
						"Content-Type: "+m_resp_info->content_type+"\r\n"
						"Date: "+time_c+"\r\n"
						"Server: "+m_resp_info->server_name+"\r\n"
						//"Connection: keep-alive\r\n"
						"Transfer-Encoding: none\r\n"
						"Access-Control-Allow-Origin: *\r\n"
						"Content-Length: "+content_len+"\r\n\r\n"+_content;
	size_t nsent = ::send(m_sock, response.c_str(), response.size(), 0);
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

int HttpSrv::Connection::getSock() {
	return m_sock;
}

void HttpSrv::Connection::parseRequests()
{
	if (readbf.size()==0)
		return;
	
	http_parser_execute(&m_parser, &m_parser_settings, readbf.c_str(), readbf.size());
}

HttpSrv::RequestPtr HttpSrv::Connection::getNextRequest()
{
	RequestPtr req;
	if (recv())
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

HttpSrv::~HttpSrv() {
	m_poolserver->stop();
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
	//std::cout << "n http conn: " << connections.size() << std::endl;
	ConnectionPtr http_conn = getHttpConn(pool_conn->m_sock);
	
	RequestPtr req = http_conn->getNextRequest();

	if (!http_conn->alive || http_conn->closing || time(0)-pool_conn->getCreateTs()>5) {
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
