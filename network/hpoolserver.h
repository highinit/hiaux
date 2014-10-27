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
		bool readmode;
		std::string ip;
		int port;
		
		boost::function<void(int)> m_onClose;
		
		uint64_t getCreateTs();
		//void recv(std::string &_bf);
		//void send(const std::string &_mess);
		void close();
		void setWriteMode();

		Connection(std::string _ip, int _port, int _sock,
				boost::function<void(int)> _onClose);
		~Connection();
	};
	
	typedef boost::shared_ptr<Connection> ConnectionPtr;
	
private:
	
	TaskLauncherPtr m_launcher;
	boost::function<void(ConnectionPtr)> m_onRead;
	boost::function<void(ConnectionPtr)> m_onWrite;
	boost::function<void(ConnectionPtr)> m_onError;
	
	int m_listen_socket;
	uint64_t m_idle_timeout;
	bool m_isrunning;
	boost::atomic<int> m_istopped;
	// socket / connection
	hiaux::hashtable<int, ConnectionPtr> m_connections;
	hAutoLock m_connections_lock;
	EventWatcherPtr m_events_watcher;
	
	std::queue<int> m_sockets_to_close_q;
	hAutoLock m_sockets_to_close_q_lock;
	
	int startServer(int port);
	
public:
    
	void onRead(int _sock, void *_opaque_info);
	void onWrite(int _sock, void *_opaque_info);
	void onError(int _sock, void *_opaque_info);
	void onAccept(int _sock_fd, void *_opaque_info);
	
	void onSetWrite(int _sock);
	
//	TaskLauncher::TaskRet handleReadTask(ConnectionPtr _conn);
	TaskLauncher::TaskRet acceptTask(int _sock);
	
	void onCloseConnection(int _sock_fd);
	TaskLauncher::TaskRet readThread();

	hPoolServer::ConnectionPtr getConnection(int _fd);

	hPoolServer(TaskLauncherPtr launcher,
			boost::function<void(ConnectionPtr)> _onRead,
			boost::function<void(ConnectionPtr)> _onWrite,
			boost::function<void(ConnectionPtr)> _onError);
	~hPoolServer();

	static int startClient(const std::string &_ip, int portno);

	void start(int port);
	void stop();
};

typedef boost::shared_ptr<hPoolServer> hPoolServerPtr;

#endif
