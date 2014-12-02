#include "Daemon.h"

void Daemon::fallDown(std::string _s) {
	std::cout << _s << std::endl;
	exit(1);
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
	
	m_required_params.push_back("log");
	m_required_params.push_back("pidfile");
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

void sigchild_handler(int signum) {
	
	int status;
	wait(&status);
}

static void empty_handler(int signal) {
	
	//std::cout << "empty handler\n";
}

void Daemon::setDefaultSignalHandlers() {
	
	signal(SIGCHLD, sigchild_handler);
	signal(SIGPIPE, empty_handler);
}

#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

int Daemon::chechLockFile(const std::string &_filename) {
	
	char	buf[16];

	int lockfile_fd = open(_filename.c_str(), O_RDWR|O_CREAT, LOCKMODE);
	
	if (lockfile_fd < 0) {
		std::cout << "cant open pid file\n";
		exit(1);
	}
	if (lockf(lockfile_fd, F_TLOCK, 100) < 0) {
		
		std::cout << "Cant lock pid file " << _filename << ". Is service already running?" << std::endl;
		
		if (errno == EACCES || errno == EAGAIN) {
			close(lockfile_fd);
			exit(1);
		}
		
		exit(1);
	}
	
	std::cout << "acquired lock file\n";
	
	ftruncate(lockfile_fd, 0);
	sprintf(buf, "%ld", (long)getpid());
	write(lockfile_fd, buf, strlen(buf)+1);
	
	return lockfile_fd;
}

void Daemon::daemonize(const std::string &_pidfile, const std::string &_logfile) {
	
	std::cout << "Daemon::daemonize\n";
	
	int					i, fd0, fd1, fd2;
	pid_t				pid;
	struct rlimit		rl;
	struct sigaction	sa;

	/*
	 * Clear file creation mask.
	 */
	umask(0);

	/*
	 * Get maximum number of file descriptors.
	 */
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
		fallDown("can't get file limit");

	/*
	 * Become a session leader to lose controlling TTY.
	 */
	if ((pid = fork()) < 0)
		fallDown("can't fork");
	else if (pid != 0) /* parent */
		exit(0);
	setsid();

	std::cout << "lose TTY ok\n";

	/*
	 * Ensure future opens won't allocate controlling TTYs.
	 */
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		fallDown("can't ignore SIGHUP");
	if ((pid = fork()) < 0)
		fallDown("can't fork");
	else if (pid != 0) /* parent */
		exit(0);

	std::cout << "Ensure future opens won't allocate controlling TTYs ok\n";

	/*
	 * Change the current working directory to the root so
	 * we won't prevent file systems from being unmounted.
	 */
//	if (chdir("/") < 0)
//		fallDown("can't change directory to /");

	int lockfile_fd = chechLockFile(_pidfile);

	/*
	 * Close all open file descriptors.
	 */
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		if (i != lockfile_fd)
			close(i);

	/*
	 * Attach file descriptors 0, 1, and 2 to /dev/null.
	 */
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);
	/*
	 * Initialize the log file.
	 */
	//openlog(cmd, LOG_CONS, LOG_DAEMON);
//	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
//		fallDown("unexpected file descriptors");
//	}
	
	//std::ofstream out(_logfile);
	freopen(_logfile.c_str(), "w", stdout);
	//std::cout.rdbuf(out.rdbuf());
	
	std::cout << "daemonize ok\n";
}

void Daemon::start(bool _daemonize) {

	if (_daemonize)
		daemonize(m_config["pidfile"], m_config["log"]);
	
	std::cout << "daemonize finished\n";
	
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
