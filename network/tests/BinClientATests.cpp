#include "BinClientATests.h"

void BinClientATests::onFinished() {
	
}

void BinClientATests::handleHttp(HttpConnectionPtr _conn, HttpRequestPtr _req) {
	
	std::cout << "BinClientATests::handleHttp " << _req->headers["Upgrade"] << std::endl;
	/*
	if (_req->headers["Upgrade"]  == "hiapi") {
		_conn->sendResponse(HttpResponse(200, HIAPI_HANDSHAKE));
		std::cout << "BinClientATests::handleHttp sending handshake\n";
	}*/
}

uint64_t testFunction(uint64_t _v0, uint64_t _v1) {
	
	return 10*_v0*_v0 + _v1;
}

void BinClientATests::handleBinary(HttpConnectionPtr _conn, CustomRequestPtr _req) {

	hiapi::server::Request *req = dynamic_cast<hiapi::server::Request*>( _req.get() );

	std::cout << "m_handle_binary: " << m_handle_binary++ << std::endl;

//	std::cout << "BinClientATests::handleBinary " << std::endl
//		<< "method: " << req->method << std::endl;
	
	std::map<std::string, std::string>::iterator p0_it = req->params.find("p0");
	std::map<std::string, std::string>::iterator p1_it = req->params.find("p1");
	std::map<std::string, std::string>::iterator end = req->params.end();
	
	TS_ASSERT (p0_it != end);
	TS_ASSERT (p1_it != end);
	
	uint64_t v0 = string_to_uint64(p0_it->second);
	uint64_t v1 = string_to_uint64(p1_it->second);
	
	std::ostringstream o;
	
	std::string resp = uint64_to_string( testFunction(v0, v1) );
	
	o << resp.size() << "\n" << resp;
	
	_conn->sendCustomResponse(o.str());
}

void BinClientATests::onCallFinished(bool _succ, const std::string &_res, uint64_t _expected_value) {
	
	//std::cout << "BinClientATests::onCallFinished " << _succ << " " << _res << std::endl;
	
	TS_ASSERT(_succ);
	
	uint64_t got_value = string_to_uint64(_res);
	
	TS_ASSERT(got_value == _expected_value);
	
	std::cout << m_got_requests++ << std::endl;
	
	
	if (m_got_requests == m_sent_requests) {
		
		std::cout << "FINISHED\n";
	}
}

CustomParserPtr BinClientATests::parserBuilder(HttpRequestPtr _req) {
	
	std::cout << "BinClientATests::parserBuilder\n";
	return CustomParserPtr(new hiapi::server::Parser(_req));
}

BinClientATests::BinClientATests() {
	
	m_handle_binary = 0;
	
	pool.reset (new hThreadPool(3));
	pool->run();
	
	launcher.reset(new TaskLauncher(pool, 3, boost::bind(&BinClientATests::onFinished, this)));
	
	int port = 1589;
	
	HttpServerPtr srv(new HttpServer(launcher,
										ResponseInfo("application/octet-stream", "network_tests"),
										boost::bind(&BinClientATests::handleHttp, this, _1, _2),
										port));
	
	CustomProtocolInfo binproto(boost::bind(&BinClientATests::parserBuilder, this, _1),
									boost::bind(&BinClientATests::handleBinary, this, _1, _2),
									HIAPI_HANDSHAKE);
	
	srv->addCustomProtocol("hiapi", binproto);
	
	sleep(1);
	
	hiapi::client::BinClientAPtr client(new hiapi::client::BinClientA(hiapi::client::BinClientA::INTERNET, "127.0.0.1", port, 1));
	
	uint64_t min_v0 = 0;
	uint64_t max_v0 = 1;
	uint64_t min_v1 = 0;
	uint64_t max_v1 = 100;
	
	m_got_requests = 0;
	m_sent_requests = 0;
	
	for (uint64_t i = min_v0; i<max_v0; i++)
		for (uint64_t j = min_v1; j<max_v1; j++)
	{
		uint64_t expected_value = testFunction(i, j);
		
		std::map<std::string, std::string> params;
		params["p0"] = uint64_to_string(i);
		params["p1"] = uint64_to_string(j);
	
		client->call("test", params, boost::bind(&BinClientATests::onCallFinished, this, _1, _2, expected_value));
		m_sent_requests++;
	}
	
	std::cout << "requests: " << m_sent_requests << std::endl; 
	
	
	sleep(1);
	
	while (1) {
		client->handleEvents();
		//srv->handleEvents();
	}
										
	std::cout << "BinClientATests::BinClientATests\n";
	pool->join();
}
