#ifndef HPOOLSERVER_H
#define HPOOLSERVER_H

#include "../threadpool/tasklauncher.h"
#include "../hrpc/hcomm/include/hsock.h"

class hPoolServer
{
public:
	class Connection
	{	
		uint64_t change_ts;
	public:
		int m_sock;
		bool closing;
		std::string ip;
		int port;
		
		uint64_t getChangeTs();
		//void recv(std::string &_bf);
		//void send(const std::string &_mess);
		void close();

		Connection(std::string _ip, int _port, int _sock);
		~Connection();
	};
	
	typedef boost::shared_ptr<Connection> ConnectionPtr;
	
private:
	TaskLauncherPtr m_launcher;
	boost::function<void(ConnectionPtr)> m_handler;
	int m_listen_socket;
	uint64_t m_idle_timeout;
	bool m_isrunning;
	std::vector<Connection> m_clients;
	
public:
    
	TaskLauncher::TaskRet Handler(ConnectionPtr client_info);
	
	TaskLauncher::TaskRet closeClientsTask();

	hPoolServer(TaskLauncherPtr launcher,
			boost::function<void(ConnectionPtr)> handler);

	TaskLauncher::TaskRet listenThread();
	void start(int port);
	void stop();
};

typedef boost::shared_ptr<hPoolServer> hPoolServerPtr;

#endif
