#include "Connection.h"

namespace hiapi {

namespace client {

Connection::Connection(int _sock):
m_sock(_sock) {
	
}

Connection::~Connection() {
	
}

void Connection::addRequest(RequestPtr _req) {
	
}

size_t Connection::getSendBufferSize() {
	
	return m_send_buffer.size();
}

void Connection::performSend() {
	
}

void Connection::performRecv() {
	
}
	
}

}
