#include "PoolServerTests.h"

void PoolServerTests::onFinished() {
	
}

void PoolServerTests::onRead(hPoolServer::ConnectionPtr _conn) {
	
	char bf[128];
	int nrecv = recv(_conn->m_sock, bf, 128, MSG_DONTWAIT);
	bf[nrecv] = '\0';
//	std::cout << "recv " << bf << std::endl;
	m_read_bf += std::string(bf);
}

void PoolServerTests::onWrite(hPoolServer::ConnectionPtr _conn) {
	std::cout  << "onWrite\n";
}

void PoolServerTests::onError(hPoolServer::ConnectionPtr _conn) {
	std::cout  << "onError\n";
}

PoolServerTests::PoolServerTests() {
	
	int port = 49101;
	
	hThreadPoolPtr pool(new hThreadPool(4));
	TaskLauncherPtr launcher(new TaskLauncher(pool, 4, boost::bind(&PoolServerTests::onFinished, this)));
	m_pool_server.reset(new hPoolServer(launcher,
										boost::bind(&PoolServerTests::onRead, this, _1),
										boost::bind(&PoolServerTests::onWrite, this, _1),
										boost::bind(&PoolServerTests::onError, this, _1)));
	
	m_pool_server->start(port);
	pool->run();
	
	sleep(1);
	
	int c_fd = hPoolServer::startClient("127.0.0.1", port);
	char chunk_0[255];
	char chunk_1[255];
	
	strcpy(chunk_0, "_request chunk0_");
	strcpy(chunk_1, "_request chunk1_");
	
	send(c_fd, chunk_0, strlen(chunk_0), 0);
	sleep(1);
	send(c_fd, chunk_1, strlen(chunk_1), 0);
	sleep(1);
	TS_ASSERT(m_read_bf == std::string(chunk_0) + chunk_1);
	
	//m_pool_server->stop();
	//pool->kill();
	//pool->join();
}

