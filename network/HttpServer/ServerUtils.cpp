#include "ServerUtils.h"

int startListening(int port) {

	char bf[255];
	struct sockaddr_in serv_addr;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	setSocketBlock(sockfd, false);

	if (sockfd < 0)
			throw new std::string("hsock_t::server: ERROR opening server socket");
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	int yes = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)  {
			//perror("setsockopt");
		std::cout << "startListening setsockopt\n";
		exit(1);
	}

	if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) == -1)  {
			
		std::cout << "startListening setsockopt\n";
		exit(1);
	}

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0)
	throw new std::string("hsock_t::server: Error server binding");

	listen(sockfd, 1024);
	return sockfd;
}

void setSocketBlock(int _fd, bool _isblock) {
	
	int flags = fcntl(_fd, F_GETFL);
	
	if (!_isblock) {
		
		flags |= O_NONBLOCK;
	} else {
		
		flags &= !O_NONBLOCK;
	}
	
	fcntl(_fd, F_SETFL, flags);
}


