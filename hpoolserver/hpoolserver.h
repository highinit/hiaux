#ifndef HPOOLSERVER_H
#define HPOOLSERVER_H

#include "../threadpool/tasklauncher.h"
//#include "../hrpc/hcomm/include/hsock.h"
//#include "../hrpc/hcomm/include/sendchannel.h"

class hPoolServer
{
public:
	class ClientInfo
	{
		int sock;
		bool closing;
		uint64_t change_ts;
	public:
		std::string ip;
		int port;
		
		uint64_t getChangeTs();
		void recv(std::string &_bf);
		void send(const std::string &_mess);
		void close();
		ClientInfo(std::string _ip, int _port, int _sock);
	};
	
private:
	TaskLauncherPtr m_launcher;
	boost::function<void(ClientInfo)> m_handler;
	int m_listen_socket;
	uint64_t m_idle_timeout;
	bool m_isrunning;
	std::vector<ClientInfo> m_clients;
	
public:
    
	TaskLauncher::TaskRet closeClientsTask();

	hPoolServer(TaskLauncherPtr launcher,
			boost::function<TaskLauncher::TaskRet(hSockClientInfo)> handler);

	TaskLauncher::TaskRet listenThread();
	void start(int port);
	void stop();
};

#endif
