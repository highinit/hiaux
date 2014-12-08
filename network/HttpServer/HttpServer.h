#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include "hiconfig.h"

#include "ResponseInfo.h"
#include "ServerUtils.h"
#include "Connection.h"
#include "Request.h"
#include "Response.h"

#include "CustomParser.h"

#include "hiaux/structs/hashtable.h"
#include "hiaux/strings/string_utils.h"

#include "hiaux/threads/tasklauncher.h"
#include "hiaux/events/EventsWatcher.h"

#include <boost/noncopyable.hpp>

class CustomProtocolInfo {
public:
	
	CustomProtocolInfo(const boost::function<CustomParserPtr(HttpRequestPtr) > &_parserBuilder,
					const boost::function<void(HttpConnectionPtr, CustomRequestPtr)> &_handler,
					const std::string &_handshake_message);
	
	boost::function<CustomParserPtr(HttpRequestPtr) > parserBuilder;
	boost::function<void(HttpConnectionPtr, CustomRequestPtr)> handler;
	std::string handshake_message;
};

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
	std::queue< std::pair<HttpConnectionPtr, std::string> > m_custom_resp_queue;
	
	void handleResponse(HttpConnectionPtr _conn);
	
	uint64_t last_cleanup;
	
	void killConnection(int _sock);
	void cleanUpDeadConnections();
	
	std::map<std::string, CustomProtocolInfo> m_customProtocols;
	
	void performRecv(int _sock);
	void performSend(int _sock);
	void performAccept(int _sock);
	
	void handleWaitingRequests(HttpConnectionPtr _conn);
	
public:
	
	void onSendResponse(int _sock, const HttpResponse &_resp);
	void onSendCustomResponse(int _sock, const std::string &_resp);
	CustomParserPtr getCustomParser(const std::string &_protocol, const HttpRequestPtr &_req, std::string &_handshake);
	
	void onRead(int _sock, void *_opaque_info);
	void onWrite(int _sock, void *_opaque_info);
	void onAccept(int _sock, void *_opaque_info);
	void onError(int _sock, void *_opaque_info);
	
	void handleEvents();
	TaskLauncher::TaskRet eventLoop();
	TaskLauncher::TaskRet customWorkerTask(HttpConnectionPtr _conn, CustomRequestPtr _req);
	TaskLauncher::TaskRet httpWorkerTask(HttpConnectionPtr _conn, HttpRequestPtr _req);
	
	// called from threadpool
	void sendResponse(HttpConnectionPtr _conn, const HttpResponse &_resp);
	
	void stop();
	
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

	void addCustomProtocol(const std::string &_protocol,
							const CustomProtocolInfo &_info);

};

typedef boost::shared_ptr<HttpServer> HttpServerPtr;

#endif
