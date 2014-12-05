#include "Connection.h"

namespace hiapi {

namespace client {

#define HTTP_DELIM "\r\n"
#define HIAPI_PROTOID "hiapi"

Connection::Connection(int _sock):
m_sock(_sock) {
	
	std::ostringstream o;
	
	o << "GET / HTTP/1.0" HTTP_DELIM
		<< "Host: hiapi" HTTP_DELIM
		<< "Connection: keep-alive" HTTP_DELIM
		<< "Upgrade: " HIAPI_PROTOID HTTP_DELIM HTTP_DELIM;

	m_send_buffer = o.str();
}

Connection::~Connection() {
	
	//std::cout << "___CLIENT Connection::~Connection\n";
	
	::close(m_sock);
	::shutdown(m_sock, SHUT_RDWR);
	
	while (!m_sent_requests.empty()) {
		
		RequestPtr cur_req = m_sent_requests.front();
		m_sent_requests.pop();
		cur_req->onFinished(false, "");
	}
	
}

bool Connection::handshaked() {
	
	return m_parser.handshaked();
}

void Connection::addRequest(RequestPtr _req) {
	
	m_sent_requests.push(_req);
	m_send_buffer.append(_req->data);
}

void Connection::onResponse(const std::string &_str) {
	
	RequestPtr cur_req = m_sent_requests.front();
	m_sent_requests.pop();
	
	cur_req->onFinished(true, _str);
}

void Connection::performSend() {
	
	int nsent = ::send(m_sock, m_send_buffer.c_str(), m_send_buffer.size(), 0);
	
	if (nsent<=0) {
		
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			std::cout << "___CLIENT Connection::performSend error: " << strerror(errno) << std::endl;
			throw LostConnectionEx();
		}
	}
	
	//if (nsent < m_send_buffer.size()) {
	
	m_send_buffer = m_send_buffer.substr(nsent, m_send_buffer.size() - nsent);
}

void Connection::performRecv() {
	
	std::string readbf;
	char bf[1025];
	int nread = ::recv(m_sock, bf, 1024, MSG_DONTWAIT);
	
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
				
				std::cout << "___CLIENT Connection::performRecv error: " << strerror(errno) << std::endl;
				throw LostConnectionEx();
			}
		} else  { // nread == 0
			
			break;
			throw LostConnectionEx();
		}
		nread = ::recv(m_sock, bf, 1024, MSG_DONTWAIT);
	}

	if (readbf.size() > 0) {
		
		m_parser.execute(readbf);
	}
	
	while (m_parser.hasResponse()) {
		
		std::string resp;
		m_parser.getResponse(resp);
		onResponse(resp);
	}
}
	
}

}
