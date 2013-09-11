#ifndef HPOOLSERVER_H
#define HPOOLSERVER_H

#include "../threadpool/threadpool.h"
#include "../hrpc/hcomm/include/hsock.h"
#include "../hrpc/hcomm/include/sendchannel.h"

class hPoolServer
{
    boost::shared_ptr<hThreadPool> m_pool;
    boost::function<void(hSockClientInfo)> m_serve_func;
    int m_socket;
    bool m_isrun;
public:
    
    hPoolServer(boost::shared_ptr<hThreadPool> pool, boost::function<void(hSockClientInfo)> serve_func);
    
    void listenThread();
    boost::shared_ptr<hThreadPool> getPool();
    void start(int port);
    void stop();
};

#endif
