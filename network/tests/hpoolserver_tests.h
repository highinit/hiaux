
#include "hiconfig.h"

#include <cxxtest/TestSuite.h>

#include "hpoolserver.h"


#include "hiaux/strings/string_utils.h"
#include "HttpSrv.h"
#include "HttpApi.h"
#include "HttpApiClient.h"

#include <boost/bind.hpp>

void onFinished() {
	
}

class hPoolServerTests : public CxxTest::TestSuite
{    

public:

	void onFinished() {
	}

	void onHttpRequest(HttpSrv::ConnectionPtr http_conn, HttpSrv::RequestPtr req)
	{
		std::cout << "onHttpRequest\n";
		hiaux::hashtable<std::string, std::string>::iterator it =
					req->values_GET.begin();
		while (it != req->values_GET.end()) {
			std::cout << it->first << "/" << it->second << std::endl;
			it++;
		}
		
		http_conn->sendResponse("SIEG HEIL SERVER!");
		http_conn->close();
	}
	
	void XtestHttpServer()
	{
		try {
		std::cout << "testHttpServer\n";
		const int port = 1234;
		hThreadPoolPtr pool (new hThreadPool(10));
		TaskLauncherPtr launcher (new TaskLauncher(
						pool, 10, boost::bind(&hPoolServerTests::onFinished, this)));
		HttpSrvPtr http_srv(new HttpSrv(launcher,
						HttpSrv::ResponseInfo("text/html; charset=utf-8",
											"highinit suggest server"),
						boost::bind(&hPoolServerTests::onHttpRequest, this, _1, _2)));
		http_srv->start(port);
		pool->run();
		pool->join();
		} catch (const char *s) {
			std::cout << "Exception: " << s << std::endl;
		}
	}
	
	void onGetStatsCalled(hiaux::hashtable<std::string, std::string> &_args, std::string& _resp) {
		_resp = "onGetStatsCalled";
	}
	
	void testHttpApiClient_simpleMethod() {
		
		std::string userid = "_userid_";
		std::string key = "_key_";
		int port = 6733;
		
		HttpApiPtr api (new HttpApi);
		api->addKey(userid, key);
		std::vector<std::string> args;
		args.push_back("ts1");
		args.push_back("ts2");
		api->addMethod("get-stats", args, boost::bind(&hPoolServerTests::onGetStatsCalled, this, _1, _2));
		
		hThreadPoolPtr pool (new hThreadPool(4));
		pool->run();
		TaskLauncherPtr launcher (new TaskLauncher(pool, 4, boost::bind(&hPoolServerTests::onFinished, this))); 
		
		HttpSrvPtr srv (new HttpSrv(launcher, HttpSrv::ResponseInfo("text/html; charset=utf-8",
										"highinit nazareth server"),
										boost::bind(&HttpApi::handle, api.get(), _1, _2)));
		
		srv->start(port);
		
		sleep(2);
		
		// make client and call
		char endpoint[255];
		sprintf(endpoint, "http://127.0.0.1:%d/", port);
		HttpApiClient c(endpoint, "_user_", "_key_");
		std::string req;
		hiaux::hashtable<std::string, std::string> params;
		params["ts1"] = "_ts1_";
		params["ts2"] = "_ts2_";
		c.call("get-stats", params, req);
		TS_ASSERT ( req == "onGetStatsCalled\r\n" );
	}
	
	void testHttpApiClient_signedMethod() {

		std::string userid = "_userid_";
		std::string key = "_key_";
		int port = 6732;
		
		HttpApiPtr api (new HttpApi);
		api->addKey(userid, key);
		std::vector<std::string> args;
		args.push_back("ts1");
		args.push_back("ts2");
		
		api->addMethodSigned("get-stats", args, boost::bind(&hPoolServerTests::onGetStatsCalled, this, _1, _2));
		
		hThreadPoolPtr pool (new hThreadPool(4));
		pool->run();
		
		TaskLauncherPtr launcher (new TaskLauncher(pool, 4, boost::bind(&hPoolServerTests::onFinished, this))); 
		
		HttpSrvPtr srv (new HttpSrv(launcher, HttpSrv::ResponseInfo("text/html; charset=utf-8",
										"highinit nazareth server"),
										boost::bind(&HttpApi::handle, api.get(), _1, _2)));
		
		srv->start(port);
		
		sleep(2);

		// make client and call
		char endpoint[255];
		sprintf(endpoint, "http://127.0.0.1:%d/", port);
		HttpApiClient c(endpoint, userid, key);
		std::string req;
		hiaux::hashtable<std::string, std::string> params;
		params["ts1"] = "_ts1_";
		params["ts2"] = "_ts2_";
		c.callSigned("get-stats", params, req);
		TS_ASSERT ( req == "onGetStatsCalled\r\n" );
	}
	
};
