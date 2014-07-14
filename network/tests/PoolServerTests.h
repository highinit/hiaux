#include <cxxtest/TestSuite.h>

#include "hpoolserver.h"

class PoolServerTests {
	
	hPoolServerPtr m_pool_server;
	std::string m_read_bf;
public:
	
	void onFinished();
	
	void onRead(hPoolServer::ConnectionPtr _conn);
	void onWrite(hPoolServer::ConnectionPtr _conn);
	void onError(hPoolServer::ConnectionPtr _conn);
	PoolServerTests();
};

