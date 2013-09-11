#include "../include/sendchannel.h"
#include "../include/hsock.h"
#include <string>

#define SIZE_MES_LENGTH 8

using namespace std;

int hSendChannel::csend(const string& str)
{
    return csend((void*)str.data(), str.size());
}

int hSendChannel::csend(const void* mem, size_t size)
{
  //  std::cout << "csend: mess_size: " << size << std::endl;
    char message_size_str[SIZE_MES_LENGTH];
    sprintf(message_size_str, "%u", size);
    send(m_socket, message_size_str, SIZE_MES_LENGTH, 0);
    send(m_socket, mem, size, 0);
}

string hSendChannel::crecv()
{
        char message_size_str[SIZE_MES_LENGTH+1];
        int err = recv(m_socket, message_size_str, SIZE_MES_LENGTH, MSG_WAITALL);
        if (err != SIZE_MES_LENGTH)
        {
                throw new string("send_channel_t::crecv error recv");
        }
       
        message_size_str[SIZE_MES_LENGTH] = '\0';

        unsigned int mess_size;
        sscanf(message_size_str, "%u", &mess_size);
   
        char buf[mess_size+1];
        err = recv(m_socket, buf, mess_size, MSG_WAITALL);
        if (err != mess_size)
        {
                throw new string("send_channel_t::crecv error recv");
        }
        buf [ mess_size ] = '\0';
        return string(buf);
}

hSendChannel::~hSendChannel()
{

}

hSendChannel::hSendChannel(int socket)
{
    m_socket = socket;
}
