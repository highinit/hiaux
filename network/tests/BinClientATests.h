#ifndef _BINCLIENTA_TESTS_
#define _BINCLIENTA_TESTS_

#include <cxxtest/TestSuite.h>
#include "HttpApi/BinClient/BinClientA.h"
#include "HttpApi/BinServer/Parser.h"
#include "HttpServer/HttpServer.h"


class BinClientATests : public CxxTest::TestSuite {

	hThreadPoolPtr pool;
	TaskLauncherPtr launcher;
	HttpServerPtr srv;
	hiapi::client::BinClientAPtr client;
	

public:
	
	CustomParserPtr parserBuilder(HttpRequestPtr _req);
	
	void onCallFinished(bool _succ, const std::string &_res);
	void onFinished();
	
	void handleHttp(HttpConnectionPtr, HttpRequestPtr);
	void handleBinary(HttpConnectionPtr _conn, CustomRequestPtr _req);
	
	BinClientATests();
	
};

#endif
