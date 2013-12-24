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
		int m_sock;
		
		uint64_t change_ts;
	public:
		bool closing;
		std::string ip;
		int port;
		
		uint64_t getChangeTs();
		void recv(std::string &_bf);
		void send(const std::string &_mess);
		void close();

		ClientInfo(std::string _ip, int _port, int _sock);
	};
	
	typedef boost::shared_ptr<ClientInfo> ClientInfoPtr;
	
private:
	TaskLauncherPtr m_launcher;
	boost::function<void(ClientInfoPtr)> m_handler;
	int m_listen_socket;
	uint64_t m_idle_timeout;
	bool m_isrunning;
	std::vector<ClientInfo> m_clients;
	
public:
    
	TaskLauncher::TaskRet Handler(ClientInfoPtr client_info);
	
	TaskLauncher::TaskRet closeClientsTask();

	hPoolServer(TaskLauncherPtr launcher,
			boost::function<void(ClientInfoPtr)> handler);

	TaskLauncher::TaskRet listenThread();
	void start(int port);
	void stop();
};

typedef boost::shared_ptr<hPoolServer> hPoolServerPtr;

#endif
