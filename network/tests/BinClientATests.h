#ifndef _BINCLIENTA_TESTS_
#define _BINCLIENTA_TESTS_

#include <cxxtest/TestSuite.h>
#include "HttpApi/BinClient/BinClientA.h"
#include "HttpApi/BinServer/Parser.h"
#include "HttpServer/HttpServer.h"


class BinClientATests : public CxxTest::TestSuite {

	uint64_t m_got_requests;
	uint64_t m_sent_requests;

	boost::atomic<uint64_t> m_handle_binary;

public:
	
	CustomParserPtr parserBuilder(HttpRequestPtr _req);
	
	void onCallFinished(bool _succ, const std::string &_res, uint64_t _expected_value);
	void onFinished();
	
	void handleHttp(HttpConnectionPtr, HttpRequestPtr);
	void handleBinary(HttpConnectionPtr _conn, CustomRequestPtr _req);
	
	BinClientATests();
	
};

#endif
