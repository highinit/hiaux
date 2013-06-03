#pragma once
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include "pthread.h"
#include "semaphore.h"



#include "signal.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/wait.h>
#include <stdarg.h>
#include <assert.h>
#include <unistd.h>
#include <netdb.h>

#ifndef SOL_TCP
#define SOL_TCP IPPROTO_TCP
#endif


using namespace std;

class HSOCK_EX
{
public:
    char *value;
    HSOCK_EX(const char *s)
    {
        value = new char [strlen(s)+1];
        strcpy(value, s);
    }
    ~HSOCK_EX()
    {
        delete [] value;
    }
};

class hsock_ifs_t
{
	public:
	int n;
	//void send(char *s);
	//void recv(char *s);
};

template <class type>
class func_arg_t
{
public:
	type *obj;
        pthread_t *th;
	int *socket;
	int port;
	char *ip;
};

template <class type>
void *func(void *a)
{
	func_arg_t<type> *b = (func_arg_t<type> *)a;
	b->obj->serve(b->th, b->socket, b->ip, b->port);
	pthread_detach(*b->th);
	return 0;
}

class hsock_t
{
        // port / socket
        map<int,int*> socket_servers;

	public:

	template <class type>
	void server(type *srv, int portno)
	{
		int *sockfd, n;
		sockfd = new int;

	//	socklen_t clilen;
		char bf[255];
		struct sockaddr_in serv_addr, cli_addr;
		*sockfd = socket(AF_INET, SOCK_STREAM, 0);
                
                
		if (*sockfd < 0)
			throw new string("hsock_t::server: ERROR opening server socket");
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(portno);
		serv_addr.sin_addr.s_addr = INADDR_ANY;
                
                int yes = 1;
                if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) 
                {
                        perror("setsockopt");
                        exit(1);
                }

		if (bind(*sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)))
		throw new string("hsock_t::server: Error server binding");
                
                map<int,int*>::iterator it = socket_servers.find(portno);
                if (it==socket_servers.end())
                {
                    pair<int,int*> map_entity (portno, sockfd) ;
                    socket_servers.insert(map_entity);
                }
                else
                {
                    throw new string("socket server already exist");
                }

		listen(*sockfd, 5);
		int clilen = sizeof(cli_addr);
		while (1)
		{
			int *newsockfd = new int;
			*newsockfd = accept(*sockfd, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);
			if (newsockfd < 0) throw new string("hsock_t::server: ERROR accepting");
			//else cout << "connected\n";
			// start thread
			func_arg_t<type> *p = new func_arg_t<type>;
			p->obj = srv;
			p->th = new pthread_t;
			p->socket = newsockfd;
			p->ip = inet_ntoa(cli_addr.sin_addr);
			p->port = cli_addr.sin_port;
                        //thr.push_back(p->th);
			pthread_create(p->th, NULL, func<type>, p);
			//pthread_join(thr[nth], NULL);
		//	nth++; &thr[nth]
		}
	}

    int *simple_server(int portno)
    {
		int *sockfd, *newsockfd, n;
		sockfd = new int;

		char bf[255];
		struct sockaddr_in serv_addr, cli_addr;

		*sockfd = socket(AF_INET, SOCK_STREAM, 0);
                
                int yes = 1;
                if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) 
                {
                        perror("setsockopt");
                        exit(1);
                }

		if (*sockfd < 0)
			throw new string("hsock_t::server: ERROR opening server socket");
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(portno);
		serv_addr.sin_addr.s_addr = INADDR_ANY;

		if (bind(*sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)))
		throw new string("hsock_t::server: Error server binding");
           

		listen(*sockfd, 5);
                
/*                map<int,int*>::iterator it = socket_servers.find(portno);
                if (it==socket_servers.end())
                {
                    socket_servers.insert(pair<int,int*>(portno, sockfd));
                }
                else
                {
                    throw new string("socket server already exist");
                }*/
                
		int clilen = sizeof(cli_addr);
		newsockfd = new int;
		*newsockfd = accept(*sockfd, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);
        if (newsockfd < 0) throw new string("hsock_t::server: ERROR accepting");
		return newsockfd;
    }

	static int *client(string ip, int portno)
	{
#ifdef WIN
		WSADATA wsaData;
		if(WSAStartup(MAKEWORD(1,1), &wsaData))
		{
		throw new HSOCK_EX("hsock_t::client: Wsastartup fail");
		return 0;
		}
#endif
                
		int *sockfd;
		sockfd = new int;
		struct sockaddr_in serv_addr;
		struct hostent *server = gethostbyname(ip.c_str());
		
		*sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
			throw new string("hsock_t::client: ERROR opening client socket");
		memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(portno);

		if (connect(*sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)))
			throw new string("hsock_t::client: ERROR connecting");
		return sockfd;
	}
        
        void close_servers()
        {
     
           // killall();
        }
        
        void close_server(int portno)
        {
             map<int,int*>::iterator it = socket_servers.find(portno);
             if (it!=socket_servers.end())
             {
                shutdown(*(it->second), 2);
                delete (it->second);
                socket_servers.erase(it);
             }
             else
             {
                throw new string("socket server dont exist");
             }
        }

	void killall()
	{
	//for (int i =0; i<thr.size(); i++)
	//	pthread_kill(*thr[i], 0);

	}

};

