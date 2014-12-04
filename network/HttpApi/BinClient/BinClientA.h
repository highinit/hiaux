#ifndef _HIAPI_BIN_CLIENTA_
#define _HIAPI_BIN_CLIENTA_

#include "hiconfig.h"

#include "hiaux/structs/hashtable.h"
#include "hiaux/threads/locks.h"
#include "hiaux/events/EventsWatcher.h"

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <string>
#include <map>
#include <queue>

#include "Request.h"
#include "Connection.h"

#include "Request.pb.h"

namespace hiapi {

namespace client {

class BinClientA : public boost::noncopyable {
public:
	
	enum Mode {
		INTERNET,
		LOCALSOCKET
	};

private:
	
	EventWatcherPtr m_events_watcher;
	
	hiaux::hashtable<int, ConnectionPtr> m_connections;
	
	hAutoLock lock;
	
	std::queue<RequestPtr> m_new_requests;

	BinClientA::Mode m_mode;
	std::string m_endpoint;

	size_t m_max_connections;

	void buildRequest(const std::string &_method, const std::map<std::string, std::string> &_params, std::string &_dump);
	void establishNewConnection();
	
public:
	
	BinClientA(BinClientA::Mode _mode, const std::string &_endpoint, size_t _max_connections = 10);
	virtual ~BinClientA();
	
	void onRead(int _sock, void *_opaque_info);
	void onWrite(int _sock, void *_opaque_info);
	void onError(int _sock, void *_opaque_info);
	void onAccept(int _sock, void *_opaque_info);
	
	void call(const std::string &_method,
				const std::map<std::string, std::string> &_params,
				boost::function<void(bool, const std::string &)> &_onFinished);
	
	void handleEvents();
};

}
}

#endif
