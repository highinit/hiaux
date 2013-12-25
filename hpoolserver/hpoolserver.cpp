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

uint64_t hPoolServer::Connection::getChangeTs()
{
	return change_ts;
}

void hPoolServer::Connection::recv(std::string &_bf)
{
	char bf[255];
	size_t nread = ::recv(m_sock, bf, 255, MSG_WAITALL);
	//std::cout << "nread " << nread << std::endl;
	//bf[nread] = '\0';
	_bf.append(bf);
}

void hPoolServer::Connection::send(const std::string &_mess)
{
	std::string bf = _mess; 
	while (bf.size() != 0) {
		size_t nsent = ::send(m_sock, bf.c_str(), bf.size(), MSG_DONTWAIT);
		bf = bf.substr(nsent, bf.size()-nsent);
	}
}

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
	if (client_info->closing) 
		return TaskLauncher::NO_RELAUNCH;
	// if timeout
	
	// ok, no kill
	return TaskLauncher::RELAUNCH;
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
				 accepted_socket));

		m_launcher->addTask(new boost::function<TaskLauncher::TaskRet()>(
			boost::bind(&hPoolServer::Handler, this, connection)));
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
