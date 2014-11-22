#ifndef _DAEMON_H_
#define _DAEMON_H_

#include "hiconfig.h"
#include "hiaux/network/HttpServer/HttpServer.h"
#include "hiaux/network/HttpOutReqDisp.h"
#include "hiaux/threads/threadpool.h"
#include "hiaux/threads/tasklauncher.h"
#include "hiaux/structs/hashtable.h"
#include "hiaux/loadconf/loadconf.h"

#include <vector>
#include <string>

#include <iostream>
#include <fstream>

#include <fcntl.h>
#include <sys/resource.h>

#include <sys/types.h>
#include <sys/stat.h>

class Daemon {

	void parseConfig(const std::string &_config_file);
protected:
	
	int m_lockfile_fd;
	
	std::vector<std::string> m_required_params;
	std::vector<std::string> m_optional_params;	

	hiaux::hashtable<std::string,std::string> m_config;

	hThreadPoolPtr m_pool;
	TaskLauncherPtr m_srv_tasklauncher;
	HttpServerPtr m_srv;

	void fallDown(std::string _s);
	void onFinished();

	void setDefaultSignalHandlers();
	int chechLockFile(const std::string &_filename);
	void daemonize(const std::string &_pidfile, const std::string &_logfile);

	void loadConfig(const std::string &_config_file);
	void startListening (size_t _nthreads, int _port);

	virtual void setParamsList(std::vector<std::string> &_required_params, std::vector<std::string> &_optional_params) = 0;
	virtual void doStart() = 0;
	virtual void connHandler(HttpConnectionPtr _conn, HttpRequestPtr _req) = 0;
	
public:
	
	Daemon();
	virtual ~Daemon();
	
	void start();
	void join();
};

typedef boost::shared_ptr<Daemon> DaemonPtr;

#endif
