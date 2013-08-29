#include "hpoolserver.h"

hPoolServer::hPoolServer(int nthreads, boost::function<void(hSockClientInfo)> f)
{
    pool = boost::shared_ptr<hThreadPool>(new hThreadPool(nthreads));
    this->f = f;
}

void hPoolServer::start(int port)
{
    
}
