#include "Connection.h"

HttpConnection::HttpConnection(int _sock, ResponseInfo _resp_info):
	sock(_sock),
	create_ts(time(0)),
	request_finished(false),
	recv_ok(true),
	m_resp_info(_resp_info),
	m_http_status_code(200)
	{
	
	request.reset(new HttpRequest);
	
	http_parser_init(&m_parser, HTTP_REQUEST);
	m_parser.data = (void*)this;
	m_parser_settings.on_message_begin = HttpConnection_onMessageBegin;
	m_parser_settings.on_url = &HttpConnection_onUrl;
	m_parser_settings.on_status = &HttpConnection_onStatus;
	m_parser_settings.on_header_field = &HttpConnection_onHeadersField;
	m_parser_settings.on_header_value = &HttpConnection_onHeadersValue;
	m_parser_settings.on_headers_complete = &HttpConnection_onHeadersComplete;
	m_parser_settings.on_body = &HttpConnection_onBody;
	m_parser_settings.on_message_complete = &HttpConnection_onMessageComplete;
	
}

HttpConnection::~HttpConnection() {

	::close(sock);
	::shutdown(sock, SHUT_RDWR);
}

bool HttpConnection::notDead() {
	
	return (time(0) - create_ts < 5) && recv_ok;
}

void HttpConnection::setHttpStatus(int code) {
	
	m_http_status_code = code;
}

void HttpConnection::addHeader(const std::string &_header) {
	
	m_headers.push_back(_header);
}

void HttpConnection::setCookie(const std::string &_name, const std::string &_value) {
	
	m_headers.push_back(std::string("Set-Cookie: ") + _name + "=" + _value + "; expires=Sat, 31 Dec 2039 23:59:59 GMT");
}

void HttpConnection::sendResponse(const std::string &_content) {
	
	char content_len_c[50];
	sprintf(content_len_c, "%d", (int)_content.size());
	std::string content_len(content_len_c);
	
	//char time_c[50];
	//sprintf(time_c, "%d", asctime(0));
	
	char time_c[50];
	sprintf(time_c, "%d", (int)time(0));
	
	std::string response = "HTTP/1.1 " + inttostr(m_http_status_code) + "\r\n"
						"Content-Type: "+m_resp_info.content_type+"\r\n"
						"Date: "+time_c+"\r\n"
						"Server: "+m_resp_info.server_name+"\r\n"
						//"Connection: keep-alive\r\n"
						"Transfer-Encoding: none\r\n"
						"Access-Control-Allow-Origin: *\r\n"
						"Connection: close\r\n";
	
	for (int i = 0; i<m_headers.size(); i++)
		response += m_headers[i] + "\r\n";
	
	response +=	"Content-Length: "+content_len+"\r\n\r\n"+_content;
	size_t nsent;
		
	//#if defined __linux__
		nsent = ::send(sock, response.c_str(), response.size(), 0);
	//#else
	//	nsent = ::send(sock, response.c_str(), response.size(), 0);
	//#endif
	
	if (nsent<=0 || nsent < response.size())
		std::cout << "HttpSrv::Connection::sendResponse SEND ERROR!!_____________"
				<< nsent << std::endl;
}

void HttpConnection::performRecv() {
	
	std::string readbf;
	char bf[2049];
	int nread = ::recv(sock, bf, 2048, MSG_DONTWAIT);
	
	while (true) {
		if (nread > 0) {
			
			bf[ nread ] = '\0';
			std::string add (bf);
			readbf.append( add );
			nread = ::recv(sock, bf, 2048, MSG_DONTWAIT);
			break;
		} 
		else { //if (errno == EWOULDBLOCK || errno == EAGAIN) {
			
			//std::cout << "HttpSrv::Connection::recv EWOULDBLOCK || EAGAIN\n";
			//performRecv();
			recv_ok = false;
			return;
		}
		//else 
		//	break;
		/*
		else if (errno == EBADF) {
			
			std::cout << "HttpSrv::Connection::recv EBADF\n";
			recv_ok = false;
			return;
		} else if (errno == ECONNREFUSED) {
			std::cout << "HttpSrv::Connection::recv ECONNREFUSED\n";
			recv_ok = false;
			return;
		} else if (errno == EFAULT) {
			std::cout << "HttpSrv::Connection::recv EFAULT\n";
			recv_ok = false;
			return;
		} else if (errno == EINTR) {
			std::cout << "HttpSrv::Connection::recv EINTR\n";
			recv_ok = false;
			return;
		} else if (errno == EINVAL) {
			std::cout << "HttpSrv::Connection::recv EINVAL\n";
			recv_ok = false;
			return;
		} else if (errno == ENOMEM) {
			std::cout << "HttpSrv::Connection::recv ENOMEM\n";
			recv_ok = false;
			return;
		} else if (errno == ENOTCONN) {
			std::cout << "HttpSrv::Connection::recv ENOTCONN\n";
			recv_ok = false;
			return;
		} else if (errno == ENOTSOCK) {
			std::cout << "HttpSrv::Connection::recv ENOTSOCK\n";
			recv_ok = false;
			return;
		} else if (nread == 0) {		
			break;
		}*/
	}
	
	//std::cout << "recv: " << readbf << std::endl;
	//m_readbf += std::string(bf);
	//std::cout << readbf;
	if (readbf.size() > 0) {
		//m_req_text += readbf;
		//std::cout << m_req_text << std::endl;
		http_parser_execute(&m_parser, &m_parser_settings, readbf.c_str(), readbf.size());
	}
	
}
