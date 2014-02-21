#include "hiconfig.h"
#include "hpoolserver.h"

using namespace std;

class PoolException : public std::exception
{
	string m_message;
	public:

	PoolException(string message)
	{
		m_message = message;
	}

	string what()
	{
		return m_message;
	}

	~PoolException() throw()
	{
		m_message.clear();
	}
};

hPoolServer::Connection::Connection(std::string _ip, int _port, int _sock,
			boost::function<void(int)> _onClose):
		ip(_ip),
		port(_port),
		m_sock(_sock),
		closing(false),
		m_onClose(_onClose)
{
}

hPoolServer::Connection::~Connection()
{
	shutdown(m_sock, SHUT_RDWR);
}

uint64_t hPoolServer::Connection::getChangeTs()
{
	return change_ts;
}

/*
void hPoolServer::Connection::recv(std::string &_bf)
{
	char bf[100];
	int nread = ::recv(m_sock, bf, 100, MSG_DONTWAIT);
	//std::cout << "nread " << nread << std::endl;
	//bf[nread] = '\0';
	if (nread>=0) {
		//std::cout << "RECIEVED: " << nread << std::endl;
		_bf.append(bf);
	}
}

void hPoolServer::Connection::send(const std::string &_mess)
{
	//std::string bf = _mess; 
	//while (bf.size() != 0) {
		size_t nsent = ::send(m_sock, _mess.c_str(), _mess.size(), MSG_DONTWAIT);
		if (nsent<=0)
			std::cout << "SEND ERROR!!_____________";
	//	bf = bf.substr(nsent, bf.size()-nsent);
//	}
}*/

void hPoolServer::Connection::close()
{
	//std::cout << "hPoolServer::Connection::close\n";
	closing = true;
	m_onClose(m_sock);
}

hPoolServer::hPoolServer(TaskLauncherPtr launcher, 
					boost::function<void(ConnectionPtr)> handler)
{
	m_launcher = launcher;
	m_handler = handler;
	m_events_watcher.reset(new EventWatcher(
			boost::bind(&hPoolServer::onRead, this, _1, _2),
			boost::bind(&hPoolServer::onWrite, this, _1, _2),
			boost::bind(&hPoolServer::onError, this, _1, _2) ));
}

void hPoolServer::onCloseConnection(int _sock_fd)
{
	hLockTicketPtr ticket = m_connections_lock.lock();
	
	hiaux::hashtable<int, ConnectionPtr>::iterator it = m_connections.find(_sock_fd);
	if (it != m_connections.end())
		m_connections.erase(it);
	m_events_watcher->delSocket(_sock_fd, NULL);
}

void hPoolServer::onRead(int _sock_fd, void *_opaque_info)
{
	//std::cout << "hPoolServer::onRead \n";
	hLockTicketPtr ticket = m_connections_lock.lock();
	hiaux::hashtable<int, ConnectionPtr>::iterator it = m_connections.find(_sock_fd);
	if (it != m_connections.end()) {
		ticket->unlock();
		m_handler(it->second);
	}
}

void hPoolServer::onWrite(int _sock_fd, void *_opaque_info)
{
	//std::cout << "hPoolServer::onWrite \n";
	hLockTicketPtr ticket = m_connections_lock.lock();
	hiaux::hashtable<int, ConnectionPtr>::iterator it = m_connections.find(_sock_fd);
	if (it != m_connections.end()) {
		ticket->unlock();
		m_handler(it->second);
	}
}

void hPoolServer::onError(int _sock_fd, void *_opaque_info)
{
	//std::cout << "hPoolServer::onError \n";
	hLockTicketPtr ticket = m_connections_lock.lock();
	hiaux::hashtable<int, ConnectionPtr>::iterator it = m_connections.find(_sock_fd);
	if (it != m_connections.end()) {
		ticket->unlock();
		m_handler(it->second);
	}
}

TaskLauncher::TaskRet hPoolServer::readThread()
{/*
	m_handler(client_info);
	
	// check to kill client
	if (client_info->closing) {
		return TaskLauncher::NO_RELAUNCH;
	}
	// if timeout
	
	// ok, no kill
	return TaskLauncher::RELAUNCH;*/
	while (m_isrunning) {
		m_events_watcher->handleEvents();
	}
}

TaskLauncher::TaskRet hPoolServer::listenThread()
{
	while (m_isrunning) {
		struct sockaddr_in cli_addr;
		size_t clilen = sizeof(cli_addr);
		int accepted_socket = accept(m_listen_socket, 
				 (struct sockaddr *) &cli_addr, 
				 (socklen_t*)&clilen);

		if (accepted_socket < 0) throw new PoolException("hsock_t::server: err accepting");

		ConnectionPtr connection(new Connection(inet_ntoa(cli_addr.sin_addr),
				 cli_addr.sin_port,
				 accepted_socket,
				boost::bind(&hPoolServer::onCloseConnection, this, _1) ));
		
		{
			hLockTicketPtr ticket = m_connections_lock.lock();
			m_connections.insert(std::pair<int, ConnectionPtr>(connection->m_sock,
														connection) );
		}
		
		//std::cout << "\n_________NEW CONNECTION\n";
		//m_launcher->addTask(new boost::function<TaskLauncher::TaskRet()>(
		//	boost::bind(&hPoolServer::onRead, this, connection)));
		
		m_events_watcher->addSocket( connection->m_sock, NULL );
		
		//return TaskLauncher::RELAUNCH;
	}
	return TaskLauncher::NO_RELAUNCH;
}

void hPoolServer::start(int port)
{
	m_isrunning = true;
	m_listen_socket = startServer(port);
	m_launcher->addTask(new boost::function<TaskLauncher::TaskRet()>(
			boost::bind(&hPoolServer::listenThread, this)));
	m_launcher->addTask(new boost::function<TaskLauncher::TaskRet()> (
			boost::bind(&hPoolServer::readThread, this)));
}

void hPoolServer::stop()
{
    shutdown(m_listen_socket, SHUT_RDWR);
    m_isrunning = 0;
}

int hPoolServer::startServer(int port)
{
	char bf[255];
	struct sockaddr_in serv_addr;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
			throw new std::string("hsock_t::server: ERROR opening server socket");
	serv_addr.sin_family = AF_INET;
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

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0)
	throw new std::string("hsock_t::server: Error server binding");

	listen(sockfd, POOLSERVER_SOCKET_CONN_QUEUE_SIZE);
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


int hPoolServer::startClient(const std::string &_ip, int portno)
{               
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server = gethostbyname(_ip.c_str());

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