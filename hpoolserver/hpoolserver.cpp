#include "hpoolserver.h"
#include "../hrpc/hcomm/include/hsock.h"

using namespace std;

class PoolException : public std::exception
{
    string m_message;
    public:
        
    PoolException(string message)
    {
        m_message = message;
    }
    
    string what()
    {
        return m_message;
    }
    
    ~PoolException() throw()
    {
        m_message.clear();
    }
    
};

hPoolServer::hPoolServer(boost::shared_ptr<hThreadPool> pool, boost::function<void(hSockClientInfo)> serve_func)
{
    m_pool = pool;
    m_serve_func = serve_func;
}

boost::shared_ptr<hThreadPool> hPoolServer::getPool()
{
    return m_pool;
}

void hPoolServer::listenThread()
{
    while (m_isrun)
    {
       struct sockaddr_in cli_addr;
       size_t clilen = sizeof(cli_addr);
       int accepted_socket = accept(m_socket, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);

       if (accepted_socket < 0) throw new PoolException("hsock_t::server: ERROR accepting");

       hSockClientInfo clinet_info(inet_ntoa(cli_addr.sin_addr), cli_addr.sin_port, accepted_socket);
       m_pool->addTask(boost::bind(m_serve_func, clinet_info));
    } 
}

void hPoolServer::start(int port)
{
    m_isrun = 1;
    m_socket = hSock::server(port);
    m_pool->addTask(boost::bind(&hPoolServer::listenThread, this));
}

void hPoolServer::stop()
{
    shutdown(m_socket, SHUT_RDWR);
    m_isrun = 0;
}
