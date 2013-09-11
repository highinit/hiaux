#ifndef SENDCHANNEL_H
#define SENDCHANNEL_H

#include <string>

class hSendChannel
{
    int m_socket;    
    
    hSendChannel() { } 
    
    public:

   
    ~hSendChannel();
    hSendChannel(int sock);

    int csend(const std::string &str);
    std::string crecv();
    int csend(const void* mem, size_t size);
};

#endif