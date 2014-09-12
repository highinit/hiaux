#include "HttpSrv.h"

HttpSrv::Connection::Connection(int _sock,
								ResponseInfoPtr _resp_info,
								boost::function<void(int, HttpSrv::RequestPtr)> _onRequest,
								boost::function<void(int)> _checkConnClose):
		m_sock(_sock),
		alive(true),
		closing(false),
		m_resp_info(_resp_info),
		m_http_status_code(200),
		m_onRequest(_onRequest),
		m_checkConnClose(_checkConnClose) {
	
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


HttpSrv::Connection::~Connection() {
//	std::cout << "http connection closed\n";
}

void HttpSrv::Connection::setHttpStatus(int _code) {
	m_http_status_code = _code;
}

void HttpSrv::Connection::addHeader(const std::string &_header) {
	
	m_headers.push_back(_header);
}

void HttpSrv::Connection::setCookie(const std::string &_name, const std::string &_value) {
	
	m_headers.push_back(std::string("Set-Cookie: ") + _name + "=" + _value + "; expires=Sat, 31 Dec 2039 23:59:59 GMT");
}

void HttpSrv::Connection::sendResponse(const std::string &_content) {
	
	
	//Sat, 28 Dec 2013 18:33:30 GMT
	char content_len_c[50];
	sprintf(content_len_c, "%d", (int)_content.size());
	std::string content_len(content_len_c);
	
	//char time_c[50];
	//sprintf(time_c, "%d", asctime(0));
	
	char time_c[50];
	sprintf(time_c, "%d", (int)time(0));
	
	std::string response = "HTTP/1.1 " + inttostr(m_http_status_code) + "\r\n"
						"Content-Type: "+m_resp_info->content_type+"\r\n"
						"Date: "+time_c+"\r\n"
						"Server: "+m_resp_info->server_name+"\r\n"
						//"Connection: keep-alive\r\n"
						"Transfer-Encoding: none\r\n"
						"Access-Control-Allow-Origin: *\r\n"
						"Connection: close\r\n";
	
	for (int i = 0; i<m_headers.size(); i++)
		response += m_headers[i] + "\r\n";
	
	response +=	"Content-Length: "+content_len+"\r\n\r\n"+_content;
	size_t nsent = ::send(m_sock, response.c_str(), response.size(), 0);
	
//	std::cout << "send: " << response << std::endl;
	
	if (nsent<=0 || nsent < response.size())
		std::cout << "HttpSrv::Connection::sendResponse SEND ERROR!!_____________"
				<< nsent << std::endl;
	close();
	m_checkConnClose(m_sock);
}

void HttpSrv::Connection::close() {
//	std::cout << "HttpSrv::Connection::close\n";
	closing = true;
}

int HttpSrv::Connection::getSock() {
	
	return m_sock;
}

void HttpSrv::Connection::performRecv() {
	
	std::string readbf;
	char bf[2049];
	int nread = ::recv(m_sock, bf, 2048, MSG_DONTWAIT);
	
	while (true) {
		if (nread > 0) {
			
			bf[ nread ] = '\0';
			std::string add (bf);
			readbf.append( add );
			nread = ::recv(m_sock, bf, 2048, MSG_DONTWAIT);
		} 
		else if (errno == EWOULDBLOCK || errno == EAGAIN) {
			
			//std::cout << "HttpSrv::Connection::recv EWOULDBLOCK || EAGAIN\n";
			break;
		}
		else if (errno == EBADF) {
			
			std::cout << "HttpSrv::Connection::recv EBADF\n";
			close();
			m_checkConnClose(m_sock);
			return;
		} else if (errno == ECONNREFUSED) {
			std::cout << "HttpSrv::Connection::recv ECONNREFUSED\n";
			close();
			m_checkConnClose(m_sock);
			return;
		} else if (errno == EFAULT) {
			std::cout << "HttpSrv::Connection::recv EFAULT\n";
			close();
			m_checkConnClose(m_sock);
			return;
		} else if (errno == EINTR) {
			std::cout << "HttpSrv::Connection::recv EINTR\n";
			close();
			m_checkConnClose(m_sock);
			return;
		} else if (errno == EINVAL) {
			std::cout << "HttpSrv::Connection::recv EINVAL\n";
			close();
			m_checkConnClose(m_sock);
			return;
		} else if (errno == ENOMEM) {
			std::cout << "HttpSrv::Connection::recv ENOMEM\n";
			close();
			m_checkConnClose(m_sock);
			return;
		} else if (errno == ENOTCONN) {
			std::cout << "HttpSrv::Connection::recv ENOTCONN\n";
			close();
			m_checkConnClose(m_sock);
			return;
		} else if (errno == ENOTSOCK) {
			std::cout << "HttpSrv::Connection::recv ENOTSOCK\n";
			close();
			m_checkConnClose(m_sock);
			return;
		} else if (nread == 0) {		
			break;
		}
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

void HttpSrv::Connection::performSend() {
	
}
