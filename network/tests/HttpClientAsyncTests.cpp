#include "HttpClientAsyncTests.h"

void HttpClientAsyncTests::onFinished() {
	
}

void HttpClientAsyncTests::onHttpRequest(HttpConnectionPtr http_conn, HttpRequestPtr req) {
	hiaux::hashtable<std::string, std::string>::iterator it =
				req->values_GET.begin();
	
	std::string resp;
	while (it != req->values_GET.end()) {
		//std::cout << it->first << "/" << it->second << std::endl;
		resp += it->second;
		it++;
	}
	
	// 0.5mb
	for (int i = 0; i<500000; i++)
		resp += "a";
	
	http_conn->sendResponse(HttpResponse(200, resp));
}

void HttpClientAsyncTests::onCalled(HttpClientAsync::JobInfo _ji) {
	//std::cout << "onCalled resp: " << _ji.resp << std::endl;
	ncalled++;
	//std::cout << "ncalled: " << ncalled << std::endl;
	//TS_ASSERT(_ji.resp == uint64_to_string( (uint64_t)_ji.userdata) );
}

HttpClientAsyncTests::HttpClientAsyncTests() {
	ncalled = 0;
	int ncalls = 2;
	const int port = 1235;
	hThreadPoolPtr pool (new hThreadPool(10));
	TaskLauncherPtr launcher (new TaskLauncher(
					pool, 10, boost::bind(&HttpClientAsyncTests::onFinished, this)));
	m_srv.reset(new HttpServer(launcher,
							ResponseInfo("text/html; charset=utf-8",
											"highinit suggest server"),
							boost::bind(&HttpClientAsyncTests::onHttpRequest, this, _1, _2),
							port));
					
	pool->run();
	//pool->join();
	sleep(1);
	
	m_cli.reset(new HttpClientAsync(boost::bind(&HttpClientAsyncTests::onCalled, this, _1)));
	
	for (uint64_t i = 0; i<ncalls; i++)
		m_cli->call((void*)i, "http://127.0.0.1:1235/?a=b&c=d");
	
	
	sleep(1);
	for (int i = 0; i<100; i++) {
		//if (i%50==0)
			//sleep(1);
		m_cli->kick();
	}
		
	//sleep(1);
		
	std::string resp;
	TS_ASSERT(ncalled == ncalls);
	
	if (ncalled == ncalls)
		exit(0);

}
