#include "Daemon.h"

void Daemon::fallDown(std::string _s) {
	std::cout << _s << std::endl;
	exit(0);
}

void Daemon::onFinished() {
	
}

void Daemon::parseConfig(const std::string &_config_file) {
	
	try {
		m_config = LoadConf::load(_config_file, m_required_params, m_optional_params);
	}
	catch (const std::string s) {
		fallDown(s);
	} catch (const char *s) {
		fallDown(s);
	} catch (...) {
		fallDown("Daemon::parseConfig: Could not parse config file: " + _config_file);
	}
}

Daemon::Daemon() {
	srand(time(0));
}

void Daemon::loadConfig(const std::string &_config_file) {
	
	setParamsList(m_required_params, m_optional_params);
	parseConfig(_config_file);
}

void Daemon::startListening (size_t _nthreads, int _port) {
	
	m_pool.reset(new hThreadPool(_nthreads));
	m_srv_tasklauncher.reset(new TaskLauncher(m_pool, _nthreads, boost::bind(&Daemon::onFinished, this)));
	m_srv.reset(new HttpServer(m_srv_tasklauncher,
							ResponseInfo("text/html; charset=utf-8", "hiaux"),
							boost::bind(&Daemon::connHandler, this, _1, _2),
							_port));
	m_pool->run();
}

void Daemon::start() {
	
	try {
	
		doStart();
		join();
	
	} catch (std::string *_s) {
		fallDown (std::string("Daemon::start exception: ") + *_s );
	} catch (const char *_s) {
		fallDown (std::string("Daemon::start exception: ") + std::string(_s) );
	} catch (std::string _s) {
		fallDown (std::string("Daemon::start exception: ") + _s );
	} catch (std::string &_s) {
		fallDown (std::string("Daemon::start exception: ") + _s );
	}
	
}

void Daemon::join() {
	
	m_pool->join();
}

Daemon::~Daemon() {
	
}
