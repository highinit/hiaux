#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include "hiconfig.h"

#include "ResponseInfo.h"
#include "ServerUtils.h"
#include "Connection.h"
#include "Request.h"
#include "Response.h"

#include "hiaux/structs/hashtable.h"
#include "hiaux/strings/string_utils.h"

#include "hiaux/threads/tasklauncher.h"
#include "hiaux/events/EventsWatcher.h"

#include <boost/noncopyable.hpp>

class HttpServer : public boost::noncopyable {
	
	TaskLauncherPtr m_launcher;
	EventWatcherPtr m_events_watcher;
	
	ResponseInfo m_resp_info;
	int m_listen_socket;
	
	boost::function<void(HttpConnectionPtr,
						HttpRequestPtr)> m_request_hdl;
	
	bool m_is_running;
	
	hiaux::hashtable<int, HttpConnectionPtr> m_reading_connections;
	//hiaux::hashtable<int, HttpConnectionPtr> m_writing_connections;
	
	hAutoLock resp_lock;
	std::queue< std::pair<HttpConnectionPtr, HttpResponse> > m_resp_queue;
	
	void handleResponse(HttpConnectionPtr _conn);
	
	uint64_t last_cleanup;
	
	void cleanUpDeadConnections();
	
public:
	
	void onSendResponse(int _sock, const HttpResponse &_resp);
		
	void onRead(int _sock, void *_opaque_info);
	void onWrite(int _sock, void *_opaque_info);
	void onError(int _sock, void *_opaque_info);
	void onAccept(int _sock_fd, void *_opaque_info);
	
	TaskLauncher::TaskRet eventLoop();
	TaskLauncher::TaskRet workerTask(HttpConnectionPtr _conn, HttpRequestPtr _req);
	
	// called from threadpool
	void sendResponse(HttpConnectionPtr _conn, const HttpResponse &_resp);
	
	HttpServer(TaskLauncherPtr launcher,
				const ResponseInfo &_resp_info,
				boost::function<void(HttpConnectionPtr,
									HttpRequestPtr)> _request_hdl,
				int _port);
	
	HttpServer(TaskLauncherPtr launcher,
				const ResponseInfo &_resp_info,
				boost::function<void(HttpConnectionPtr,
									HttpRequestPtr)> _request_hdl,
				const std::string &_localsocket);
};

typedef boost::shared_ptr<HttpServer> HttpServerPtr;

#endif
