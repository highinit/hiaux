#ifndef HSOCK_H
#define HSOCK_H
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include "pthread.h"
#include "semaphore.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "signal.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netinet/tcp.h>

#include <sys/wait.h>
#include <stdarg.h>
#include <assert.h>
#include <unistd.h>
#include <netdb.h>

#define CONN_QUEUE_SIZE 1000

class hSockClientInfo
{
public:

    std::string m_ip;
    int m_port;
    int m_sock;
    
    hSockClientInfo(const std::string ip, const int port, const int sock)
    {
        m_ip = ip;
        m_port = port;
        m_sock = sock;
    }
    
    hSockClientInfo& operator=(const hSockClientInfo &c)
    {
        m_ip = c.m_ip;
        m_port = c.m_port;
        m_sock = c.m_sock;
        return *this;
    }
};

#ifndef SOL_TCP
#define SOL_TCP IPPROTO_TCP
#endif

class hSock
{
    public:

    static hSockClientInfo* acceptClient(int listen_socket)
    {
        struct sockaddr_in cli_addr;
        int clilen = sizeof(cli_addr);
        int newsockfd = accept(listen_socket, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);
        if (newsockfd < 0) throw new std::string("hsock_t::server: ERROR accepting");
        return new hSockClientInfo(inet_ntoa(cli_addr.sin_addr), cli_addr.sin_port, newsockfd);        
    }
        
    static int server(int port)
    {
        char bf[255];
        struct sockaddr_in serv_addr;

        int sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (sockfd < 0)
                throw new std::string("hsock_t::server: ERROR opening server socket");
        serv_addr.sin_family = PF_INET;
        serv_addr.sin_port = htons(port);
        serv_addr.sin_addr.s_addr = INADDR_ANY;

        int yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) 
        {
                perror("setsockopt");
                exit(1);
        }
        
        if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) == -1) 
        {
                perror("setsockopt");
                exit(1);
        }

        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))==0)
        throw new std::string("hsock_t::server: Error server binding");

        listen(sockfd, CONN_QUEUE_SIZE);
        return sockfd;
        
        /*while (1)
        {
         *      struct sockaddr_in cli_addr;
                int *newsockfd = new int;
                *newsockfd = accept(*sockfd, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);
                if (newsockfd < 0) throw new string("hsock_t::server: ERROR accepting");

                func_arg_t<type> *p = new func_arg_t<type>;
                p->obj = srv;
                p->th = new pthread_t;
                p->socket = newsockfd;
                p->ip = inet_ntoa(cli_addr.sin_addr);
                p->port = cli_addr.sin_port;
                pthread_create(p->th, NULL, func<type>, p);

        }*/
    }
 
    
    static int client(std::string ip, int portno)
    {               
        int sockfd;
        struct sockaddr_in serv_addr;
        struct hostent *server = gethostbyname(ip.c_str());

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0)
                throw new std::string("hsock_t::client: ERROR opening client socket");
        
        int yes = 1;
        if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) == -1) 
        {
               // perror("setsockopt");
               // exit(1);
        }

        memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(portno);

        if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)))
                throw new std::string("hsock_t::client: ERROR connecting");
        return sockfd;
    }

};

#endif