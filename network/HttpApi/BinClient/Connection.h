
#ifndef _HIAPI_BINCLIENT_CONNECTION_
#define _HIAPI_BINCLIENT_CONNECTION_

#include "hiconfig.h"

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <string>
#include <queue>

#include "Parser.h"
#include "Request.h"

namespace hiapi {

namespace client {
	
class Connection {

	ResponseParser m_parser;

	int m_sock;
	std::queue<RequestPtr> m_sent_requests;
	std::string m_send_buffer;

public:
	
	Connection(int _sock);
	virtual ~Connection();
	
	size_t getSendBufferSize();
	
	void addRequest(RequestPtr _req);
	void performSend();
	void performRecv();
};

typedef boost::shared_ptr<Connection> ConnectionPtr;
	
}
}

#endif
