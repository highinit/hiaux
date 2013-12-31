#include "hpoolserver.h"
#include <boost/bind.hpp>

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

hPoolServer::Connection::Connection(std::string _ip, int _port, int _sock):
		ip(_ip),
		port(_port),
		m_sock(_sock),
		closing(false)
{
}

hPoolServer::Connection::~Connection()
{
	shutdown(m_sock, SHUT_RDWR);
//	std::cout << "pool connection closed\n";
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
	closing = 1;
}

hPoolServer::hPoolServer(TaskLauncherPtr launcher, 
					boost::function<void(ConnectionPtr)> handler)
{
    m_launcher = launcher;
    m_handler = handler;
}

TaskLauncher::TaskRet hPoolServer::Handler(ConnectionPtr client_info)
{
	m_handler(client_info);
	
	// check to kill client
	if (client_info->closing) {
		return TaskLauncher::NO_RELAUNCH;
	}
	// if timeout
	
	// ok, no kill
	return TaskLauncher::RELAUNCH;
}

TaskLauncher::TaskRet hPoolServer::listenThread()
{
	if (m_isrunning) {
		struct sockaddr_in cli_addr;
		size_t clilen = sizeof(cli_addr);
		int accepted_socket = accept(m_listen_socket, 
				 (struct sockaddr *) &cli_addr, 
				 (socklen_t*)&clilen);

		if (accepted_socket < 0) throw new PoolException("hsock_t::server: err accepting");

		ConnectionPtr connection(new Connection(inet_ntoa(cli_addr.sin_addr),
				 cli_addr.sin_port,
				 accepted_socket));
		std::cout << "\n_________NEW CONNECTION\n";
		m_launcher->addTask(new boost::function<TaskLauncher::TaskRet()>(
			boost::bind(&hPoolServer::Handler, this, connection)));
		return TaskLauncher::RELAUNCH;
	}
	return TaskLauncher::NO_RELAUNCH;
}

void hPoolServer::start(int port)
{
	m_isrunning = true;
	m_listen_socket = hSock::server(port);
	m_launcher->addTask(new boost::function<TaskLauncher::TaskRet()>(
			boost::bind(&hPoolServer::listenThread, this)));
	//m_launcher->addTask(boost::bind(&hPoolServer::closeClientsTask, this));
}

void hPoolServer::stop()
{
    shutdown(m_listen_socket, SHUT_RDWR);
    m_isrunning = 0;
}
