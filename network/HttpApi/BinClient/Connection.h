
#ifndef _HIAPI_BINCLIENT_CONNECTION_
#define _HIAPI_BINCLIENT_CONNECTION_

#include "hiconfig.h"

#include "hiaux/network/HttpServer/ServerUtils.h"

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

#include <string>
#include <queue>

#include <sstream>

#include "Parser.h"
#include "Request.h"

namespace hiapi {

namespace client {

class LostConnectionEx {
public:
	
};

class Connection {

	ResponseParser m_parser;

	int m_sock;
	std::queue<RequestPtr> m_sent_requests;
	std::string m_send_buffer;

	void onResponse(const std::string &_str);

public:
	
	enum State {
		HANDSHAKING,
		JUST_HANDSHAKED,
		ACTIVE
	};

	State state;
	
	Connection(int _sock);
	virtual ~Connection();
	
	void onHandshaked();
	
	void addRequest(RequestPtr _req);
	void performSend();
	void performRecv();
};

typedef boost::shared_ptr<Connection> ConnectionPtr;
	
}
}

#endif
