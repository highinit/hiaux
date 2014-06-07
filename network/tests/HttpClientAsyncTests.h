
#include <cxxtest/TestSuite.h>
#include "HttpClientAsync.h"
#include "HttpSrv.h"

class HttpClientAsyncTests : public CxxTest::TestSuite {
	HttpSrvPtr m_srv;
	HttpClientAsyncPtr m_cli;
	int ncalled;
public:
	void onFinished();
	void onCalled(HttpClientAsync::JobInfo _ji);
	void onHttpRequest(HttpSrv::ConnectionPtr http_conn, HttpSrv::RequestPtr req);
	HttpClientAsyncTests();
};
