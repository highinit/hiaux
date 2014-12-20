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

static void sigchild_handler(int signum) {
	
	pid_t pid;
	int status;
	while ( (pid = waitpid(-1, &status, WNOHANG)) > 0);
}

static void empty_handler(int signal) {
	
}

void Daemon::setDefaultSignalHandlers() {
	
	struct sigaction	sa;
	sa.sa_handler = sigchild_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	
	sigaction(SIGCHLD, &sa, NULL);
		
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	
	sigaction(SIGPIPE, &sa, NULL);
}

#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

int Daemon::checkLockFile(const std::string &_filename) {
	
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

void Daemon::startWatcher() {
	
	sigset_t sigset;
	siginfo_t siginfo;
	sigemptyset(&sigset);
	
	sigaddset(&sigset, SIGQUIT);
	sigaddset(&sigset, SIGINT);
	sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGCHLD);
	
	sigprocmask(SIG_BLOCK, &sigset, NULL);
	
	pid_t child_pid;
	bool need_start = true;
	
	for (;;) {
		
		if (need_start)
			child_pid = fork();
		
		need_start = false;
		
		if (child_pid == -1) {
			
		} else if (child_pid == 0) { // child
			
			startWorker();
			exit(0);
			
		} else { // parent
			
			int got_sig;
			if (sigwait(&sigset, &got_sig) == -1) {
				
				std::cout << "Daemon::startWatcher sigwait error\n";
			}
			
			if (got_sig == SIGCHLD) {
				
				std::cout << "Daemon::startWatcher SIGCHLD\n";
				int status;
				pid_t pid;
				while ( (pid = waitpid(-1, &status, WNOHANG)) > 0);
				need_start = true;
				
			} else {
				
				kill(child_pid, SIGKILL);
				exit(0);
			}
		}
	}
}

void Daemon::startWorker() {
	
	try {
		
		setDefaultSignalHandlers();
		doStart();
		//join();
	
	} catch (std::string *_s) {
		fallDown (std::string("Daemon::startWorker exception: ") + *_s );
	} catch (const char *_s) {
		fallDown (std::string("Daemon::startWorker exception: ") + std::string(_s) );
	} catch (std::string _s) {
		fallDown (std::string("Daemon::startWorker exception: ") + _s );
	} catch (std::string &_s) {
		fallDown (std::string("Daemon::startWorker exception: ") + _s );
	} catch (std::exception &_e) {
		fallDown (std::string("Daemon::startWorker exception: ")  + _e.what());
	} catch (...) {
		fallDown (std::string("Daemon::startWorker unknown exception: ") );
	}
}

void Daemon::daemonize(const std::string &_pidfile, const std::string &_logfile) {
	
	std::cout << "Daemon::daemonize\n";
	
	int					i, fd0, fd1, fd2;
	pid_t				pid;
	struct rlimit		rl;
	struct sigaction	sa;

	umask(0);

	// Get maximum number of file descriptors.
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
		fallDown("Daemon::daemonize: can't get file limit");

	// Become a session leader to lose controlling TTY.
	if ((pid = fork()) < 0)
		fallDown("Daemon::daemonize: can't fork");
	else if (pid != 0) /* parent */
		exit(0);
	setsid();

	//std::cout << "Daemon::daemonize: lose TTY ok\n";
	
	// Ensure future opens won't allocate controlling TTYs.
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		fallDown("Daemon::daemonize: can't ignore SIGHUP");
	if ((pid = fork()) < 0)
		fallDown("Daemon::daemonize: can't fork");
	else if (pid != 0) /* parent */
		exit(0);

	//std::cout << "Daemon::daemonize: Ensure future opens won't allocate controlling TTYs ok\n";

	// Change the current working directory to the root so we won't prevent file systems from being unmounted.
//	if (chdir("/") < 0)
//		fallDown("can't change directory to /");

	int lockfile_fd = checkLockFile(_pidfile);

	// Close all open file descriptors.
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		if (i != lockfile_fd)
			close(i);

	
	// Attach file descriptors 0, 1, and 2 to /dev/null.
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);
	
	// Initialize the log file.
	//openlog(cmd, LOG_CONS, LOG_DAEMON);
//	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
//		fallDown("unexpected file descriptors");
//	}
	
	//std::ofstream out(_logfile);
	freopen(_logfile.c_str(), "w", stdout);
	//std::cout.rdbuf(out.rdbuf());
	std::cout << "Daemon::daemonize ok\n";
}

int Daemon::doStop() {
	
	std::ifstream d (m_config["pidfile"].c_str());
	std::string pidstr;
	d >> pidstr;
	std::cout << "Daemon::doStop process: " << pidstr << std::endl;
	
	return system( (std::string("kill ") + pidstr).c_str() );
}

void Daemon::start(const std::string &_config_name, int argc, char** argv) {
	
	namespace po = boost::program_options;
	po::options_description desc("Allowed options");
	
	std::string config_path;
	bool interactive = false;
	
	Command command = START;
	
	desc.add_options()
	("help,h", "Show help")
	("interactive,i", "Interactive mode. Not daemon")
	("stop", "Stop service")
	("restart", "Restart service")
	("config,c", po::value<std::string>(&config_path), "Specify config file");
	
	try {
	
		po::variables_map vm;
		po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
	    po::store(parsed, vm);
	    po::notify(vm);
		
		if (vm.count("help")) {
			
			std::cout << desc << std::endl;
			exit(0);
		}
		
		if (vm.count("config")) {
			
		}
		
		if (vm.count("stop")) {
			
			command = STOP;
		}
		
		if (vm.count("restart")) {
			
			command = RESTART;
		}
		
		if (vm.count("interactive")) {
			
			interactive = true;
		}
	
	} catch (...) {
		
		std::cout << desc << std::endl;
	}
	
	if (config_path == "")
		config_path = _config_name;
	
	loadConfig(config_path);
	
	if (command == START) {
	
		std::cout << "Daemon::start config: " << config_path << std::endl;
	
		if (!interactive)
			daemonize(m_config["pidfile"], m_config["log"]);
	
		startWatcher();
	}
	else if (command == STOP) {
		
		exit(doStop());
	}
	else if (command == RESTART) {
		
		doStop();
		sleep(2);
		
		if (!interactive)
			daemonize(m_config["pidfile"], m_config["log"]);
	
		startWatcher();
	}
}

Daemon::~Daemon() {
	
}
