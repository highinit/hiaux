#ifndef _BINCLIENTPARSER_TESTS_
#define _BINCLIENTPARSER_TESTS_

#include <cxxtest/TestSuite.h>
#include "HttpApi/BinClient/BinClientA.h"
#include "HttpApi/BinServer/Parser.h"
#include "HttpServer/HttpServer.h"

#include "HttpApi/BinClient/Parser.h"

class BinClientParserTests : public CxxTest::TestSuite {

	void onHandshaked();
	
public:
	
	BinClientParserTests();
};

#endif
