#ifndef _SERVER_UTILS_H_
#define _SERVER_UTILS_H_

#include "hiconfig.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string>

#include <iostream>

class CannotConnectEx {
	
	std::string mess;
	
public:
	
	CannotConnectEx();
	CannotConnectEx(const char *_s);
	
	std::string what();
};

void setSocketBlock(int _fd, bool _isblock);

int startListening(int port);
int startListening(const std::string &_localsocket);

int connectSocket(const std::string &_ip, int port);

#endif
