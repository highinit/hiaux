#ifndef _SERVER_UTILS_H_
#define _SERVER_UTILS_H_

#include "hiconfig.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>

#include <string>



int startListening(int port);

#endif
