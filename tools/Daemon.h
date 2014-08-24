#ifndef _DAEMON_H_
#define _DAEMON_H_

#include "hiconfig.h"
#include "hiaux/network/HttpSrv.h"
#include "hiaux/network/HttpOutReqDisp.h"
#include "hiaux/threads/threadpool.h"
#include "hiaux/threads/tasklauncher.h"
#include "hiaux/structs/hashtable.h"
#include "hiaux/loadconf/loadconf.h"

#include <vector>
#include <string>

class Daemon {

	void parseConfig(const std::string &_config_file);
protected:
	
	std::vector<std::string> m_required_params;
	std::vector<std::string> m_optional_params;	

	hiaux::hashtable<std::string,std::string> m_config;

	hThreadPoolPtr m_pool;
	TaskLauncherPtr m_srv_tasklauncher;
	HttpSrvPtr m_srv;

	void fallDown(std::string _s);
	void onFinished();

	void loadConfig(const std::string &_config_file);
	void startListening (size_t _nthreads, int _port);

	virtual void setParamsList(std::vector<std::string> &_required_params, std::vector<std::string> &_optional_params) = 0;
	virtual void doStart() = 0;
	virtual void connHandler(HttpSrv::ConnectionPtr _conn, HttpSrv::RequestPtr _req) = 0;
	
public:
	
	Daemon();
	virtual ~Daemon();
	
	void start();
	void join();
};

typedef boost::shared_ptr<Daemon> DaemonPtr;

#endif
