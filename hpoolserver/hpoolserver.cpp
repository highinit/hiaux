#include "hpoolserver.h"
#include "../hrpc/hcomm/include/hsock.h"

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

hPoolServer::ClientInfo::ClientInfo(std::string _ip, int _port, int _sock):
		ip(_ip),
		port(_port),
		sock(_sock),
		closing(false)
{
}

uint64_t hPoolServer::ClientInfo::getChangeTs()
{
	return change_ts;
}

std::string hPoolServer::ClientInfo::recv(std::string &_bf)
{

}

std::string hPoolServer::ClientInfo::send(const std::string &_mess)
{

}

void hPoolServer::ClientInfo::close()
{
	closing = 1;
}

hPoolServer::hPoolServer(TaskLauncherPtr launcher, 
					boost::function<TaskLauncher::TaskRet(hSockClientInfo)> handler)
{
    m_launcher = launcher;
    m_handler = handler;
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

		ClientInfo clinet_info(inet_ntoa(cli_addr.sin_addr),
				 cli_addr.sin_port,
				 accepted_socket);

		m_launcher->addTask(boost::bind(m_handler, clinet_info));
	}
	return TaskLauncher::NO_RELAUNCH;
}

TaskLauncher::TaskRet hPoolServer::closeClientsTask()
{
	for (int i = 0; i<m_clients.size(); i++) {
		m_handler(m_clients[i]);
		if (time(0) - m_clients[i].getChangeTs() > m_idle_timeout) {
			// убиваем клиента
		}
	}
}

void hPoolServer::start(int port)
{
	m_isrunning = true;
	m_listen_socket = hSock::server(port);
	m_launcher->addTask(boost::bind(&hPoolServer::listenThread, this));
	m_launcher->addTask(boost::bind(&hPoolServer::closeClientsTask, this));
}

void hPoolServer::stop()
{
    shutdown(m_listen_socket, SHUT_RDWR);
    m_isrunning = 0;
}
