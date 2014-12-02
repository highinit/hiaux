#include "HttpOutReqDispTests.h"

TestRequester::TestRequester(boost::function<void(int, int, const std::string&)> _onCall,
				boost::function<void(int, int, const std::string&, const std::string&)> _onCallPost,
				boost::function<void(int)> _onFinished,
				int id,
				boost::function<void()> _onWrongResp,
				boost::function<void()> _onResp):
	HttpOutRequestDisp::Requester(_onCall, _onCallPost, _onFinished),
	m_onWrongResp(_onWrongResp),
	m_onResp(_onResp) {
		
}

void TestRequester::onCallDone (int _callid, bool _success, const std::string &_resp) {
	//std::cout << "TestRequester::onCallDone resp: " << _resp << std::endl;
	m_onResp();
	
	if (!_success) {
		std::cout << "!_success\n";
	}
	
	if (_resp != inttostr(m_id)) {
		std::cout << "wrong resp got " << _resp << " must: " << m_id << std::endl;
	//	m_onWrongResp();
	}

	finished();
}

void TestRequester::start() {
//	std::cout << "TestRequester::start\n";
	call(0, "http://localhost:1236/?param=" + inttostr(m_id));
}

void HttpOutReqDispTests::onFinished() {
	
}

void HttpOutReqDispTests::onHttpRequest(HttpConnectionPtr http_conn, HttpRequestPtr req) {
	//std::cout << "HttpOutReqDispTests::onHttpRequest\n";
	hiaux::hashtable<std::string, std::string>::iterator it =
				req->values_GET.begin();
	
	std::string resp;
	while (it != req->values_GET.end()) {
		//std::cout << it->first << "/" << it->second << std::endl;
		resp += it->second;
		it++;
	}
	
	http_conn->sendResponse( HttpResponse(200, resp) );
//	http_conn->close();
}

void HttpOutReqDispTests::onResp() {
	nresps++;
}

void HttpOutReqDispTests::onWrongResp() {
	TS_ASSERT(false);
}

HttpOutReqDispTests::HttpOutReqDispTests() {
	nresps = 0;
	const int port = 1236;
	const int nthreads = 16;
	hThreadPoolPtr pool (new hThreadPool(nthreads));
	TaskLauncherPtr launcher (new TaskLauncher(
					pool, nthreads, boost::bind(&HttpOutReqDispTests::onFinished, this)));
	m_srv.reset(new HttpServer(launcher,
					ResponseInfo("text/html; charset=utf-8",
										"highinit suggest server"),
					boost::bind(&HttpOutReqDispTests::onHttpRequest, this, _1, _2),
						port));
					
	pool->run();
	//pool->join();
	sleep(1);
	
	m_req_disp.reset(new HttpOutRequestDisp(launcher));
	
	int ncalls = 200;
	
	for (int i = 0; i<ncalls; i++) {
	
		TestRequesterPtr requester(new TestRequester(boost::bind(&HttpOutRequestDisp::onCall, m_req_disp.get(), _1, _2, _3),
													boost::bind(&HttpOutRequestDisp::onCallPost, m_req_disp.get(), _1, _2, _3, _4),
													boost::bind(&HttpOutRequestDisp::onRequesterFinished, m_req_disp.get(), _1),
													i,
													boost::bind(&HttpOutReqDispTests::onWrongResp, this),
													boost::bind(&HttpOutReqDispTests::onResp, this)));
		m_req_disp->addRequester(requester);
	}


	while (nresps < ncalls) {
		//if (nresps % 1000 == 0)
		//	std::cout << "nresps: " << nresps << std::endl;
		sleep(1);
	}

	//std::cout << "nresps: " << nresps << std::endl;
	//TS_ASSERT(nresps == ncalls);
	/*
	for (int i = 0; i<100; i++) {
		if (i%50==0)
			sleep(1);
		m_req_disp->kick();
	}*/
}

