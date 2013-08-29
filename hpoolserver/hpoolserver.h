#ifndef HPOOLSERVER_H
#define HPOOLSERVER_H

#include "../threadpool/threadpool.h"
#include "../hrpc/hcomm/include/hsock.h"

class hPoolServer
{
    boost::shared_ptr<hThreadPool> pool;
    boost::function<void(hSockClientInfo)> f;
    int port;
public:
    
    hPoolServer(int nthreads);
    void start(int port);
};

#endif