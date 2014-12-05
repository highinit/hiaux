
#ifndef _HTTPCLIENTASYNC_TESTS_H_
#define _HTTPCLIENTASYNC_TESTS_H_

#include <cxxtest/TestSuite.h>
#include "HttpClientAsync.h"
#include "HttpServer.h"

class HttpClientAsyncTests : public CxxTest::TestSuite {
	//HttpServerPtr m_srv;
	//HttpClientAsyncPtr m_cli;
	int ncalled;
public:
	void onFinished();
	void onCalled(HttpClientAsync::JobInfo _ji);
	void onHttpRequest(HttpConnectionPtr http_conn, HttpRequestPtr req);
	HttpClientAsyncTests();
};

#endif
