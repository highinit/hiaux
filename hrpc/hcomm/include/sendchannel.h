#pragma once
#include "common.h"
#include "hqueue.h"

#define OK 0
#define FAULT 1

class send_channel_t
{
    int sock;    
    public:

    send_channel_t();
    ~send_channel_t();

    send_channel_t(int sock);

    int csend(string str);
    string crecv();

    int csend(void* mem, size_t size);
    void *crecv(size_t size);
    
    void connect(string ip, int port);
    void close();

};

