#include "HttpClientAsyncTests.h"

void HttpClientAsyncTests::onFinished() {
	
}

void HttpClientAsyncTests::onHttpRequest(HttpSrv::ConnectionPtr http_conn, HttpSrv::RequestPtr req) {
	hiaux::hashtable<std::string, std::string>::iterator it =
				req->values_GET.begin();
	
	std::string resp;
	while (it != req->values_GET.end()) {
		//std::cout << it->first << "/" << it->second << std::endl;
		resp += it->second;
		it++;
	}
	
	http_conn->sendResponse(resp);
	http_conn->close();
}

void HttpClientAsyncTests::onCalled(HttpClientAsync::JobInfo _ji) {
	//std::cout << "onCalled resp: " << _ji.resp << std::endl;
	ncalled++;
	TS_ASSERT(_ji.resp == uint64_to_string( (uint64_t)_ji.userdata) );
}

HttpClientAsyncTests::HttpClientAsyncTests() {
	ncalled = 0;
	const int port = 1235;
	hThreadPoolPtr pool (new hThreadPool(10));
	TaskLauncherPtr launcher (new TaskLauncher(
					pool, 10, boost::bind(&HttpClientAsyncTests::onFinished, this)));
	m_srv.reset(new HttpSrv(launcher,
					HttpSrv::ResponseInfo("text/html; charset=utf-8",
										"highinit suggest server"),
					boost::bind(&HttpClientAsyncTests::onHttpRequest, this, _1, _2)));
					
	m_srv->start(port);
	pool->run();
	//pool->join();
	sleep(1);
	
	m_cli.reset(new HttpClientAsync(boost::bind(&HttpClientAsyncTests::onCalled, this, _1)));
	
	for (uint64_t i = 0; i<10; i++)
		m_cli->call((void*)i, "http://localhost:1235/?param=" + uint64_to_string(i));
	
	
	sleep(1);
	for (int i = 0; i<100; i++) {
		if (i%50==0)
			sleep(1);
		m_cli->kick();
	}
		
	//sleep(1);
		
	std::string resp;
	TS_ASSERT(ncalled == 10);

}
