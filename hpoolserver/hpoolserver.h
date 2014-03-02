#ifndef HPOOLSERVER_H
#define HPOOLSERVER_H

#include "hiconfig.h"
#include "hiaux/structs/hashtable.h"
#include "hiaux/threads/tasklauncher.h"
#include "hiaux/events/EventsWatcher.h"

#include <boost/bind.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>

class hPoolServer
{
public:
	class Connection
	{	
		uint64_t create_ts;
	public:
		int m_sock;
		bool closing;
		std::string ip;
		int port;
		
		boost::function<void(int)> m_onClose;
		
		uint64_t getCreateTs();
		//void recv(std::string &_bf);
		//void send(const std::string &_mess);
		void close();

		Connection(std::string _ip, int _port, int _sock,
				boost::function<void(int)> _onClose);
		~Connection();
	};
	
	typedef boost::shared_ptr<Connection> ConnectionPtr;
	
private:
	
	TaskLauncherPtr m_launcher;
	boost::function<void(ConnectionPtr)> m_handler;
	int m_listen_socket;
	uint64_t m_idle_timeout;
	bool m_isrunning;
	// socket / connection
	hiaux::hashtable<int, ConnectionPtr> m_connections;
	hAutoLock m_connections_lock;
	EventWatcherPtr m_events_watcher;
	
	int startClient(const std::string &_ip, int portno);
	int startServer(int port);
	
public:
    
	void onRead(int _sock, void *_opaque_info);
	void onWrite(int _sock, void *_opaque_info);
	void onError(int _sock, void *_opaque_info);
	
	void onCloseConnection(int _sock_fd);
	TaskLauncher::TaskRet readThread();

	hPoolServer(TaskLauncherPtr launcher,
			boost::function<void(ConnectionPtr)> handler);

	TaskLauncher::TaskRet listenThread();
	void start(int port);
	void stop();
};

typedef boost::shared_ptr<hPoolServer> hPoolServerPtr;

#endif
