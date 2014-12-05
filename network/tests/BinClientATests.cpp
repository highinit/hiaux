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

void BinClientATests::handleBinary(HttpConnectionPtr _conn, CustomRequestPtr _req) {

	std::cout << "BinClientATests::handleBinary " << std::endl;
}

void BinClientATests::onCallFinished(bool _succ, const std::string &_res) {
	
	std::cout << "BinClientATests::onCallFinished " << _succ << " " << _res << std::endl;
}

CustomParserPtr BinClientATests::parserBuilder(HttpRequestPtr _req) {
	
	std::cout << "BinClientATests::parserBuilder\n";
	return CustomParserPtr(new hiapi::server::Parser(_req));
}

BinClientATests::BinClientATests() {
	
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
	
	
	std::map<std::string, std::string> params;
	params["p0"] = "v0";
	params["p1"] = "v1";
	
	client->call("test", params, boost::bind(&BinClientATests::onCallFinished, this, _1, _2));
	
	sleep(1);
	
	while (1) {
		client->handleEvents();
		srv->handleEvents();
	}
										
	std::cout << "BinClientATests::BinClientATests\n";
	pool->join();
}
