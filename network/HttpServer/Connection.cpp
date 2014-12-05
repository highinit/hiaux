#include "Connection.h"

HttpConnection::HttpConnection(int _sock,
								ResponseInfo _resp_info,
								const boost::function<void(int, const HttpResponse &)> &_on_send_response,
								const boost::function<void(int, const std::string &)> &_on_send_custom_response,
								const boost::function<CustomParserPtr(const std::string &_protocol, const HttpRequestPtr &_req, std::string &_handshake)> &_getCustomParser):
	sock(_sock),
	create_ts(time(0)),
	last_activity_ts(create_ts),
	request_finished(false),
	alive(true),
	ever_sent(false),
	keepalive(false),
	custom_protocol(false),
	m_resp_info(_resp_info),
	m_http_status_code(200),
	waiting_last_handling(false),
	m_on_send_response(_on_send_response),
	m_on_send_custom_response(_on_send_custom_response),
	m_getCustomParser(_getCustomParser) {
	
	m_cur_http_request.reset(new HttpRequest);
	
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
	
	//std::cout << "HttpConnection::HttpConnection\n";
}

HttpConnection::~HttpConnection() {

	//std::cout << "HttpConnection::~HttpConnection\n";

	::close(sock);
	::shutdown(sock, SHUT_RDWR);
}

void HttpConnection::resetHttpParser() {
	
	http_parser_init(&m_parser, HTTP_REQUEST);
}

bool HttpConnection::notDead() {
	
	//std::cout << "HttpConnection::notDead " << ((time(0) - create_ts < 5) && alive)
	//	<< " alive: " << alive << std::endl; 
	
	return (time(0) - create_ts < 5) && alive;
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

bool HttpConnection::checkUpgrade(HttpRequestPtr request) {
	
	std::map<std::string, std::string>::iterator it = request->headers.find("Upgrade");
	
	if (it == request->headers.end())
		return false;
	
	//std::cout << "HttpConnection::checkUpgrade " << it->second << std::endl;;
	//std::cout << "Keep-alive: " << keepalive << std::endl;
	
	std::string handshake_message;
	
	m_custom_parser = m_getCustomParser(it->second, request, handshake_message);
	
	if (m_custom_parser) {
		
		//std::cout << "HttpConnection::checkUpgrade potocol upgraded, set custom parser\n";
		
		custom_protocol = true;
		custom_protocol_id = it->second;
		
		sendCustomResponse(handshake_message);
		
		return true;
	}
	
	return false;
}

void HttpConnection::renderResponse(const HttpResponse &_resp, std::string &_response) {
	
	char content_len_c[50];
	sprintf(content_len_c, "%d", (int)_resp.body.size());
	std::string content_len(content_len_c);
	
	char time_c[50];
	sprintf(time_c, "%d", (int)time(0));
	
	std::string keepalive_header = "Connection: Keep-Alive\r\n";
	
	if (!keepalive)
		keepalive_header = "Connection: close\r\n";
	
	_response = "HTTP/1.1 " + inttostr(_resp.code) + "\r\n"
						"Content-Type: "+m_resp_info.content_type+"\r\n"
						"Date: "+time_c+"\r\n"
						"Server: "+m_resp_info.server_name+"\r\n"
						+keepalive_header+
						"Transfer-Encoding: none\r\n"
						"Access-Control-Allow-Origin: *\r\n";
	
//	for (int i = 0; i<m_headers.size(); i++) {
//		_response += m_headers[i] + "\r\n";
//	}
	
	_response += "Content-Length: "+content_len+"\r\n\r\n"+_resp.body;
}

void HttpConnection::sendResponse(const HttpResponse &_resp) {
	
	m_on_send_response(sock, _resp);
}

void HttpConnection::sendCustomResponse(const std::string &_resp) {
	
	m_on_send_custom_response(sock, _resp);
}

void HttpConnection::addResponse(const HttpResponse &_resp) {
	
	std::string dump;
	
	renderResponse(_resp, dump);
	m_resps.push(dump);
}

void HttpConnection::addCustomResponse(const std::string &_resp) {
	
	m_resps.push(_resp);
}

bool HttpConnection::performSend() {
	
	if (m_send_buffer.size() == 0) {
		
		// one response sent
		if (ever_sent && !keepalive) {
			
			//std::cout << "ever_sent && !keepalive\n";
			alive = false;
			return false;
		}
		
		if (m_resps.size() == 0) {

			return false;
		}
		
		m_send_buffer = m_resps.front();
		m_resps.pop();
	}
	//setSocketBlock(sock, false);
	int nsent = ::send(sock, m_send_buffer.c_str(), m_send_buffer.size(), 0);
	
	ever_sent = true;
	
	if (nsent<=0) {
		
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			alive = false;
			std::cout << "nsent<=0 \n";
		}
			
		return false;
	}
	
	if (nsent < m_send_buffer.size()) {
	
		m_send_buffer = m_send_buffer.substr(nsent, m_send_buffer.size() - nsent);
		return false;
	}
	
	m_send_buffer.clear();
	return true;
}

std::string getRecvError(int _errno) {
	
//	if (_errno == )
	return "";
}

void HttpConnection::performRecv() {
	
	std::string readbf;
	char bf[1025];
	int nread = ::recv(sock, bf, 1024, MSG_DONTWAIT);
	
	while (true) {
		if (nread > 0) {
			
			bf[ nread ] = '\0';
			readbf.append( bf, nread );
		} 
		else if (nread < 0) { //
			
			if (errno == EWOULDBLOCK || errno == EAGAIN) {
				
				break;
			}
			else {
			
				std::cout << "nread < 0: " << strerror(errno) << std::endl;
				
				alive = false;
				return;
			}
		} 
		else  { // nread == 0
			
			//std::cout << "nread == 0\n";
			//alive = false;
			break;
		}
		nread = ::recv(sock, bf, 1024, MSG_DONTWAIT);
	}

	if (readbf.size() > 0) {
		//m_req_text += readbf;
		//std::cout << m_req_text << std::endl;
		
		if (!custom_protocol)
			http_parser_execute(&m_parser, &m_parser_settings, readbf.c_str(), readbf.size());
		else
			m_custom_parser->execute(readbf);
	}
	
	if (!custom_protocol) {
	
		if (request_finished) {
	
			http_requests.push(m_cur_http_request);
			m_cur_http_request.reset(new HttpRequest);
			request_finished = false;
		}
		
	} else {
		
		while (m_custom_parser->hasRequest()) {
		
			custom_requests.push(m_custom_parser->getRequest());
		}
		//request_finished = false;
	}
	
}
