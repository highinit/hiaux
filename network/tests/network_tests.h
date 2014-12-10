
#ifndef _NETWORK_TESTS_
#define _NETWORK_TESTS_


#include "hiconfig.h"

#include <cxxtest/TestSuite.h>

#include "hpoolserver.h"

#include "hiaux/strings/string_utils.h"
#include "HttpServer.h"
#include "HttpApi/HttpApi.h"
#include "HttpApi/HttpApiClient.h"

#include <boost/bind.hpp>

#include "PoolServerTests.h"
#include "HttpClientAsyncTests.h"
#include "HttpOutReqDispTests.h"

#include "BinClientParserTests.h"
#include "BinServerParserTests.h"
#include "BinClientATests.h"

#include "HttpServer.h"

void onFinished() {
	
}

class NetworkTests : public CxxTest::TestSuite
{

public:

	void onFinished() {
	}

	void XtestPoolServer() {
		//PoolServerTests();
		exit(0);
	}

	void onHttpRequest(HttpConnectionPtr http_conn, HttpRequestPtr req)
	{
		std::cout << "onHttpRequest\n";
		std::map<std::string, std::string>::iterator it =
					req->values_GET.begin();
		while (it != req->values_GET.end()) {
			std::cout << it->first << "/" << it->second << std::endl;
			it++;
		}
		
		http_conn->sendResponse( HttpResponse(200, "SERVER RESPONSE!") );
	}
	
	void XtestHttpServer()
	{
		try {
			//std::cout << "testHttpServer\n";
			const int port = 34934;
			hThreadPoolPtr pool (new hThreadPool(10));
			TaskLauncherPtr launcher (new TaskLauncher(
							pool, 10, boost::bind(&NetworkTests::onFinished, this)));
			HttpServerPtr http_srv(new HttpServer(launcher,
							ResponseInfo("text/html; charset=utf-8",
												"highinit suggest server"),
							boost::bind(&NetworkTests::onHttpRequest, this, _1, _2),
							port));
			
			pool->run();
			
			sleep(100);
			
			pool->join();
	
	/*		sleep(1);
			
			HttpClient cli;
			std::string resp;
			//cli.callSimple("http://localhost:1234/?zhi=123&est=37", resp);
	
			int c_fd = hPoolServer::startClient("127.0.0.1", port);
			char chunk_0[512];
			char chunk_1[512];
			char chunk_2[512];
			
			strcpy(chunk_0, "GE");
	*/			
		//	strcpy(chunk_1,	"T /developer/js/common.js HTTP/1.1\n"
		//					"Host: marketing.adobe.com\n"
		//					"Connection: keep-alive\n"
		//					"Cache-Control: max-age=0\n"
		//					"Accept: */*\n"
		//					"User-Age");
	/*						
			strcpy(chunk_2,	"nt: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.153 Safari/537.36\n"
							"Referer: https://marketing.adobe.com/developer/en_US/documentation/data-insertion/r-sample-http-get\n"
							"Accept-Encoding: gzip,deflate,sdch\n"
							"Accept-Language: en-US,en;q=0.8,ru;q=0.6\n\n");

			send(c_fd, chunk_0, strlen(chunk_0), 0);
			sleep(1);
			send(c_fd, chunk_1, strlen(chunk_1), 0);
			sleep(1);
			send(c_fd, chunk_2, strlen(chunk_2), 0);
			sleep(1);
			
			char bf[1024];
			
			size_t nrecv = recv(c_fd, bf, 1024, 0);
			bf[nrecv] = '\0';
			
			std::cout << "response: " << bf << std::endl;
			
			TS_ASSERT(std::string(bf) == "SERVER RESPONSE!")
			//std::cout << "resp: " << resp;
			exit(0);
		*/
		} catch (const char *s) {
			std::cout << "Exception: " << s << std::endl;
		}
	}
	/*
	std::string onGenError(const std::string &_mess) {
		return _mess;
	}
	
	void onGetStatsCalled(std::map<std::string, std::string> &_args, std::string& _resp) {
		_resp = "onGetStatsCalled\r\n";
	}
	
	void onGetStatsCalledAsync(std::map<std::string, std::string> &_args,  boost::function< void(const std::string&)> _onDone) {

		_onDone ("onGetStatsCalled\r\n");
	}
	
	void XtestHttpApiClient_simpleMethod() {
		
		std::string userid = "_userid_";
		std::string key = "_key_";
		int port = 6733;
		
		HttpApiPtr api (new HttpApi(boost::bind(&NetworkTests::onGenError, this, _1)));
		api->addUser(userid, key);
		std::vector<std::string> args;
		args.push_back("ts1");
		args.push_back("ts2");
		api->addMethod("get-stats", args, boost::bind(&NetworkTests::onGetStatsCalled, this, _1, _2));
		
		hThreadPoolPtr pool (new hThreadPool(4));
		pool->run();
		TaskLauncherPtr launcher (new TaskLauncher(pool, 4, boost::bind(&NetworkTests::onFinished, this))); 
		
		HttpServerPtr srv (new HttpServer(launcher, ResponseInfo("text/html; charset=utf-8",
										"highinit nazareth server"),
										boost::bind(&HttpApi::handle, api.get(), _1, _2),
										port));
		
		
		sleep(2);
		
		// make client and call
		char endpoint[255];
		sprintf(endpoint, "http://127.0.0.1:%d/", port);
		HttpApiClient c(endpoint, "_user_", "_key_");
		std::string req;
		std::map<std::string, std::string> params;
		params["ts1"] = "_ts1_";
		params["ts2"] = "_ts2_";
		c.call("get-stats", params, req);
		TS_ASSERT ( req == "onGetStatsCalled\r\n" );
	}
	
	void XtestHttpApiClient_signedMethod() {

		std::string userid = "_userid_";
		std::string key = "_key_";
		int port = 6732;
		
		HttpApiPtr api (new HttpApi(boost::bind(&NetworkTests::onGenError, this, _1)));
		api->addUser(userid, key);
		std::vector<std::string> args;
		args.push_back("ts1");
		args.push_back("ts2");
		
		api->addMethodSigned("get-stats", args, boost::bind(&NetworkTests::onGetStatsCalled, this, _1, _2));
		
		hThreadPoolPtr pool (new hThreadPool(4));
		pool->run();
		
		TaskLauncherPtr launcher (new TaskLauncher(pool, 4, boost::bind(&NetworkTests::onFinished, this))); 
		
		HttpServerPtr srv (new HttpServer(launcher, ResponseInfo("text/html; charset=utf-8",
										"highinit nazareth server"),
										boost::bind(&HttpApi::handle, api.get(), _1, _2),
										port));
		
		
		sleep(2);

		// make client and call
		char endpoint[255];
		sprintf(endpoint, "http://127.0.0.1:%d/", port);
		HttpApiClient c(endpoint, userid, key);
		std::string req;
		std::map<std::string, std::string> params;
		params["ts1"] = "_ts1_";
		params["ts2"] = "_ts2_";
		c.callSigned("get-stats", params, req);
	//	std::cout << "req: " << req << std::endl;
		TS_ASSERT ( req == "onGetStatsCalled\r\n" );
	}
	
	void XtestHttpApiClient_signedMethodAsync() {

		std::string userid = "_userid_";
		std::string key = "_key_";
		int port = 6732;
		
		HttpApiPtr api (new HttpApi(boost::bind(&NetworkTests::onGenError, this, _1)));
		api->addUser(userid, key);
		std::vector<std::string> args;
		args.push_back("ts1");
		args.push_back("ts2");
		
		api->addMethodSignedAsync("get-stats", args, boost::bind(&NetworkTests::onGetStatsCalledAsync, this, _1, _2));
		
		hThreadPoolPtr pool (new hThreadPool(4));
		pool->run();
		
		TaskLauncherPtr launcher (new TaskLauncher(pool, 4, boost::bind(&NetworkTests::onFinished, this))); 
		
		HttpServerPtr srv (new HttpServer(launcher, ResponseInfo("text/html; charset=utf-8",
										"highinit nazareth server"),
										boost::bind(&HttpApi::handle, api.get(), _1, _2),
										port));
		
		
		sleep(2);

		// make client and call
		char endpoint[255];
		sprintf(endpoint, "http://127.0.0.1:%d/", port);
		HttpApiClient c(endpoint, userid, key);
		std::string req;
		std::map<std::string, std::string> params;
		params["ts1"] = "_ts1_";
		params["ts2"] = "_ts2_";
		c.callSigned("get-stats", params, req);
	//	std::cout << "req: " << req << std::endl;
		TS_ASSERT ( req == "onGetStatsCalled\r\n" );
	}
	
	void XtestHttpApiClient_postData() {

		std::string userid = "_userid_";
		std::string key = "_key_";
		int port = 6732;
		
		HttpApiPtr api (new HttpApi(boost::bind(&NetworkTests::onGenError, this, _1)));
		api->addUser(userid, key);
		std::vector<std::string> args;
		args.push_back("ts1");
		args.push_back("ts2");
		
		api->addMethodSigned("get-stats", args, boost::bind(&NetworkTests::onGetStatsCalled, this, _1, _2));
		
		hThreadPoolPtr pool (new hThreadPool(4));
		pool->run();
		
		TaskLauncherPtr launcher (new TaskLauncher(pool, 4, boost::bind(&NetworkTests::onFinished, this))); 
		
		HttpServerPtr srv (new HttpServer(launcher, ResponseInfo("text/html; charset=utf-8",
										"highinit nazareth server"),
										boost::bind(&HttpApi::handle, api.get(), _1, _2),
										port));
		
		
		sleep(1);

		// make client and call
		char endpoint[255];
		sprintf(endpoint, "http://127.0.0.1:%d/", port);
		HttpApiClient c(endpoint, userid, key);
		std::string req;
		std::map<std::string, std::string> params;
		params["ts1"] = "_ts1_";
		params["ts2"] = "_ts2_";
		c.callSignedPost("get-stats", params, req);
		std::cout << "req: " << req << std::endl;
		TS_ASSERT ( req == "onGetStatsCalled\r\n" );
	}
	*/
	void XtestHttpClientAsync() {
		HttpClientAsyncTests();
	}
	
	void XtestHttpOutReqDisp() {
		HttpOutReqDispTests();
	}
	
	void testBinClientParser() {
		BinClientParserTests();
	}
	
	void testBinServerParser() {
		BinServerParserTests();
	}
	
	void testBinClientA() {
		BinClientATests();
	}
};

#endif
