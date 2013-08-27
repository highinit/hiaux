 
#include <cxxtest/TestSuite.h> 
#include <time.h>
#include <errno.h>
#include "../include/hsock.h"

void getSockError()
{
    if (errno == EACCES)
    {
        std::cout << "EACCES\n";
    }
    if (errno == EAGAIN)
    {
        std::cout << "EAGAIN\n";
    }
    if (errno == EWOULDBLOCK)
    {
        std::cout << "EWOULDBLOCK\n";
    }
    if (errno == EBADF)
    {
        std::cout << "EBADF\n";
    }
    if (errno == ECONNRESET)
    {
        std::cout << "ECONNRESET\n";
    }
    if (errno == EDESTADDRREQ)
    {
        std::cout << "EDESTADDRREQ\n";
    }
    if (errno == EFAULT)
    {
        std::cout << "EFAULT\n";
    }
    if (errno == EINTR)
    {
        std::cout << "EINTR\n";
    }
    if (errno == EINVAL)
    {
        std::cout << "EINVAL\n";
    }
    if (errno == EISCONN)
    {
        std::cout << "EISCONN\n";
    }
    if (errno == EMSGSIZE)
    {
        std::cout << "EMSGSIZE\n";
    }
    if (errno == ENOBUFS)
    {
        std::cout << "ENOBUFS\n";
    }
    if (errno == ENOMEM)
    {
        std::cout << "ENOMEM\n";
    }
    if (errno == ENOTCONN)
    {
        std::cout << "ENOTCONN\n";
    }
    if (errno == ENOTSOCK)
    {
        std::cout << "ENOTSOCK\n";
    }
    if (errno == EOPNOTSUPP)
    {
        std::cout << "EOPNOTSUPP\n";
    }
    if (errno == EPIPE)
    {
        std::cout << "EPIPE\n";
    }
}

void *serv_th(void *a)
{
    pair<int, std::string>* arg = (pair<int, std::string>*)a; 
    int port =  arg->first;
    int serv = hSock::server(port);
    hSockClientInfo* clinfo = hSock::acceptClient(serv);
    int got = 0;

    std::string res = "";
    char bf1[15]; char bf2[15];
    strcpy(bf2, "1234");
    
    got += recv(clinfo->sock, bf1, 5, MSG_WAITALL);
    send(clinfo->sock, bf1, 4, MSG_DONTWAIT);
    
    
    bf1[5] = '\0';
    res += std::string(bf1);
    arg->second = res;
    getSockError();
    return 0;
}

class MyTest : public CxxTest::TestSuite
{
public:
  
    void testSimpleMess()
    {
        try
        {
            int port = 7849;
            pair<int, std::string> arg;
            arg.first = port;
            
            pthread_t th;
            pthread_create(&th, 0, serv_th, (void*)&arg);
            sleep(1);
            int cli = hSock::client("127.0.0.1", port);

            char bf[15];
            strcpy(bf, "12345");
            
            char bf1[20];
            strcpy(bf1, "");
            
            timespec start, finish;
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
            send(cli, bf, 5, MSG_DONTWAIT);
            recv(cli, bf1, 4, MSG_WAITALL);
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, &finish);
            
            std::cout << "ping: " << finish.tv_sec-start.tv_sec << "s " << finish.tv_nsec - start.tv_nsec << " ns" << std::endl;
            
            bf1[4] = '\0';
            TS_ASSERT(std::string(bf1) == "1234");      
            TS_ASSERT(arg.second == std::string(bf));
            
        } catch (std::string *s)
        {      
            std::cout << s->c_str () << std::endl;
        }
    }
  
};
