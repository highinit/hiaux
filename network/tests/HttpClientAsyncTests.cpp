#include "HttpClientAsyncTests.h"

void HttpClientAsyncTests::onFinished() {
	
}

void HttpClientAsyncTests::onHttpRequest(HttpSrv::ConnectionPtr http_conn, HttpSrv::RequestPtr req) {
	hiaux::hashtable<std::string, std::string>::iterator it =
				req->values_GET.begin();
	while (it != req->values_GET.end()) {
		//std::cout << it->first << "/" << it->second << std::endl;
		it++;
	}
	
	http_conn->sendResponse("SERVER RESPONSE!");
	http_conn->close();
}

void HttpClientAsyncTests::onCalled(HttpClientAsync::JobInfo _ji) {
	std::cout << "onCalled\n";
}

HttpClientAsyncTests::HttpClientAsyncTests() {
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
	
	m_cli->call(NULL, "http://localhost:1235/");
	
	for (int i = 0; i<5; i++) {
		sleep(1);
		m_cli->kick();
	}
	
	std::string resp;
	TS_ASSERT(resp == "SERVER RESPONSE!");
}
