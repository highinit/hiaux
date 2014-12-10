#include "Connection.h"

namespace hiapi {

namespace client {

#define HTTP_DELIM "\r\n"
#define HIAPI_PROTOID "hiapi"

Connection::Connection(int _sock):
m_sock(_sock),
state(HANDSHAKING),
m_parser(boost::bind(&Connection::onHandshaked, this)),
m_last_activity_ts(time(0)),
m_keepalive_period(5) {

	std::ostringstream o;
	
	o << "GET / HTTP/1.0" HTTP_DELIM
		<< "Host: hiapi" HTTP_DELIM
		<< "Connection: keep-alive" HTTP_DELIM
		<< "Upgrade: " HIAPI_PROTOID HTTP_DELIM HTTP_DELIM;

	m_send_buffer = o.str();
	performSend();
	
//	std::cout << "___CLIENT Connection::Connection\n";
}

Connection::~Connection() {
	
//	std::cout << "___CLIENT Connection::~Connection\n";
	
	::close(m_sock);
	::shutdown(m_sock, SHUT_RDWR);
	
	while (!m_sent_requests.empty()) {
		
		RequestPtr cur_req = m_sent_requests.front();
		m_sent_requests.pop();
		cur_req->onFinished(false, "");
	}
}

void Connection::onHandshaked() {
	
	//std::cout << "Connection::onHandshaked\n";
	state = ACTIVE;
}

void Connection::addRequest(RequestPtr _req) {
	
	std::ostringstream o;
	o << _req->data.size() << "\n"
		<< _req->data;
	
	m_sent_requests.push(_req);
	m_send_buffer.append(o.str());
}

void Connection::onResponse(const std::string &_str) {
	
	RequestPtr cur_req = m_sent_requests.front();
	m_sent_requests.pop();
	
	cur_req->onFinished(true, _str);
}

void Connection::checkKeepAlive(uint64_t _now) {
	
	if ( (m_send_buffer.size() == 0) && (_now - m_last_activity_ts > m_keepalive_period)) {
		
		m_last_activity_ts = _now;
		m_send_buffer.append(KEEPALIVE_MESSAGE);
		performSend();
	}
}

void Connection::performSend() {
	
	//std::cout << "Connection::performSend\n";
	
	//m_last_activity_ts = time(0);
	
	if (m_send_buffer.size() == 0)
		return;
	
	//std::cout << "sending " << m_send_buffer << std::endl;
	
	int nsent = ::send(m_sock, m_send_buffer.c_str(), m_send_buffer.size(), 0);
	
	if (nsent<=0) {
		
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			
			throw LostConnectionEx();
		}
	}
	
	//if (nsent < m_send_buffer.size()) {
	
	if (m_send_buffer.size() == nsent) {
		
		m_send_buffer.clear();
		return;
	}
	//std::cout << "m_send_buffer: " << m_send_buffer << std::endl;
	m_send_buffer = m_send_buffer.substr(nsent, m_send_buffer.size() - nsent);
}

void Connection::performRecv() {
	
	//m_last_activity_ts = time(0);
	
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
				
				//std::cout << "___CLIENT Connection::performRecv error: " << strerror(errno) << std::endl;
				throw LostConnectionEx();
			}
		} else  { // nread == 0
			
			//break;
			throw LostConnectionEx();
		}
		nread = ::recv(m_sock, bf, 1024, MSG_DONTWAIT);
	}

	if (readbf.size() > 0) {
		
		//std::cout << "_CLIENT Connection::performRecv |" << readbf <<  "|" <<  std::endl;
		
		m_parser.execute(readbf);
	}
	
	while (m_parser.hasResponse()) {
	
		//std::cout << "___CLIENT got response\n";
		
		std::string resp;
		m_parser.getResponse(resp);
		onResponse(resp);
	}
}
	
}

}
