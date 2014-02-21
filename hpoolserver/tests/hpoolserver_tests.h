
#include "hiconfig.h"

#include <cxxtest/TestSuite.h>

#include "hpoolserver.h"


#include "hiaux/strings/string_utils.h"
#include "HttpSrv.h"

class hPoolServerTests : public CxxTest::TestSuite
{    

public:
   
    class CheckInitConnection
    {
//       int connected;
    public:     
/*
        void serve(hSockClientInfo cli_info)
        {
            connected++;
        }

        CheckInitConnection()
        {
            connected = 0;
            boost::shared_ptr<hThreadPool> pool(new hThreadPool(4));
            hPoolServer pool_server(pool, boost::bind(&CheckInitConnection::serve, this, _1)); 
            int port = 6666;
            pool_server.start(port);
            pool->run();

            int client = hSock::client("127.0.0.1", port);
            int client2 = hSock::client("127.0.0.1", port);
            sleep(1);
            TS_ASSERT(connected == 2);
        }   

 */
	};
    
    void XtestCheckInitConnection()      //////////////////////////
    {
        CheckInitConnection();
    }
    
    class CheckSendSimple
    {
    public:
        /*
        void serve(hSockClientInfo cli_info)
        {
            hSendChannel server (cli_info.m_sock);
            std::string str = server.crecv();
            TS_ASSERT(str == "4815162342");           
        }
        
        CheckSendSimple()
        {
            boost::shared_ptr<hThreadPool> pool(new hThreadPool(2));
            hPoolServer pool_server(pool, boost::bind(&CheckSendSimple::serve, this, _1)); 
            int port = 6667;
            pool_server.start(port);
            pool->run();
            sleep(1);
            hSendChannel client ( hSock::client("127.0.0.1", port) );
            client.csend("4815162342");
            sleep(1);
        }*/
    };
    
    void XtestSendSimple()               //////////////////////////
    {
        CheckSendSimple();
    }
	
	void onFinished()
	{
	
	}
	
	/*
	
	void poolServerHandler(hPoolServer::ConnectionPtr conn)
	{
		std::string bf = "";
		
	//	while (bf.find('\n')==std::string::npos)
		
		conn->recv(bf);
		if (bf!="") {
			std::cout << bf << std::endl;
			std::tr1::unordered_map<std::string, std::string> values_GET;
			
			
			std::string content = "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">"
								"<html><body>SERVER HEIL!</body></html>";

			char content_len_c[50];
			sprintf(content_len_c, "%d", content.size());
			std::string content_len(content_len_c); 
			std::string response = "HTTP/1.1 200 OK\r\n"
						"Content-Type: text/html; charset=utf-8\r\n"
						"Date: Sat, 28 Dec 2013 18:33:30 GMT\r\n"
						"Server: highinit suggest server\r\n"
						"Connection: keep-alive\r\n"
						"Transfer-Encoding: none\r\n"
						"Content-Length: "+content_len+"\n\n"+content+"\r\n";


			char buf[response.size()];
			utf8_to_latin9(buf, response.c_str(), response.size());
			//std::cout << "\n\nRESPONSE: \n" << response;
			conn->send(buf);
		}
		//conn->close();
	}
	
	void XtestPoolServer()
	{
		try {
		hThreadPool *pool = new hThreadPool(10);
		TaskLauncherPtr launcher (new TaskLauncher(
						pool, 10, boost::bind(&hPoolServerTests::onFinished, this)));
		hPoolServerPtr server(new hPoolServer(launcher,
						boost::bind(&hPoolServerTests::poolServerHandler, this, _1)));
		const int port = 12345;
		server->start(port);
		pool->run();
		launcher->setNoMoreTasks();
		
		
		pool->join();
		}
		catch (std::string *s) {
			std::cout << s->c_str();
		}
		
	}
	
	void XtestWebSocketServer()
	{
		const int port = 12345;
		hThreadPool *pool = new hThreadPool(10);
		TaskLauncherPtr launcher (new TaskLauncher(
						pool, 10, boost::bind(&hPoolServerTests::onFinished, this)));
		
		WebSocketSrvPtr websocket_srv(new WebSocketSrv(launcher));
		websocket_srv->start(port);
		pool->run();
		//launcher->setNoMoreTasks();
		pool->join();
	}*/
	
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
	
	void testHttpServer()
	{
		try {
		std::cout << "testHttpServer\n";
		const int port = 1234;
		hThreadPool *pool = new hThreadPool(10);
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
	
};
