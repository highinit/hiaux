#ifndef _HIAPI_BIN_CLIENTA_
#define _HIAPI_BIN_CLIENTA_

#include "hiconfig.h"

#include "hiaux/structs/hashtable.h"
#include "hiaux/threads/locks.h"
#include "hiaux/events/EventsWatcher.h"

#include "hiaux/network/HttpServer/ServerUtils.h"

#include "hiaux/network/HttpApi/HiApiClientA.h"

#include "hiaux/network/HttpApi/BinClient/Request.h"
#include "hiaux/network/HttpApi/BinClient/Connection.h"

#include "hiaux/network/HttpApi/BinClient/Request.pb.h"

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <string>
#include <map>
#include <queue>

namespace hiapi {

namespace client {

class BinClientA : public HiApiClientA, public boost::noncopyable {
public:
	
	enum Mode {
		INTERNET,
		LOCALSOCKET
	};

private:
	
	EventWatcherPtr m_events_watcher;
	
	std::map<int, ConnectionPtr> m_connections;
	std::map<int, ConnectionPtr> m_free_connections;
	
	hAutoLock lock;
	
	// pop from onSend
	std::queue<RequestPtr> m_new_requests;

	BinClientA::Mode m_mode;
	std::string m_ip;
	int m_port;

	size_t m_max_connections;

	void establishNewConnection();
	void onLostConnection(ConnectionPtr _conn);
	void reinitConnections();
	
	void putRequestsToFreeConnections();
	void checkKeepAlive();
	
	void performRecv(ConnectionPtr _conn);
	void performSend(ConnectionPtr _conn);
	
	void removeConnection(ConnectionPtr _conn);
	
public:
	
	BinClientA(BinClientA::Mode _mode, const std::string &_ip, int _port, size_t _max_connections = 50);
	virtual ~BinClientA();
	
	static void buildRequest(const std::string &_method, const std::map<std::string, std::string> &_params, std::string &_dump);
	
	void onRead(int _sock, void *_opaque_info);
	void onWrite(int _sock, void *_opaque_info);
	void onError(int _sock, void *_opaque_info);
	void onAccept(int _sock, void *_opaque_info);
	
	virtual void call(const std::string &_method,
				const std::map<std::string, std::string> &_params,
				const boost::function<void(bool, const std::string &)> &_onFinished);
	
	virtual void callSigned (const std::string &_method, const std::map<std::string, std::string> &_params, const boost::function<void(bool, const std::string &)> &_onFinished);
	
	void handleEvents();
};

typedef boost::shared_ptr<BinClientA> BinClientAPtr;

}
}

#endif
