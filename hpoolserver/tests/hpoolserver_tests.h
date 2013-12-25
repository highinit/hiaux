
#include <cxxtest/TestSuite.h>

#include "../hpoolserver.h"
#include "../websocket/WebSocketSrv.h"

#include "../../hrpc/hcomm/include/hsock.h"
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
    
    void testCheckInitConnection()      //////////////////////////
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
    
    void testSendSimple()               //////////////////////////
    {
        CheckSendSimple();
    }
	
	void onFinished()
	{
	
	}
	
	void poolServerHandler(hPoolServer::ConnectionPtr conn)
	{
		std::string bf = "";
		while (bf.find('\n')==std::string::npos)
		{
			conn->recv(bf);
			std::cout << bf << std::endl;
		}
		conn->send("SERVER HEIL!");
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
		
		sleep(1);
		
			for (int i = 0; i<10; i++) {
				int cli = hSock::client("127.0.0.1", port);
				char bf[255];
				sprintf(bf, "CLIENT SIEG!\n");
				::send(cli, bf, 255, 0);
				::recv(cli, bf, 255, 0);
				std::cout << bf << std::endl;
			}
		pool->join();
		}
		catch (std::string *s) {
			std::cout << s->c_str();
		}
		
	}
	
	void testWebSocketServer()
	{
		const int port = 12345;
		hThreadPool *pool = new hThreadPool(10);
		TaskLauncherPtr launcher (new TaskLauncher(
						pool, 10, boost::bind(&hPoolServerTests::onFinished, this)));
		
		WebSocketSrvPtr websocket_srv(new WebSocketSrv(launcher));
		websocket_srv->start(port);
		pool->run();
		launcher->setNoMoreTasks();
		pool->join();
	}
	
};
