#ifdef __linux__

#include "hiconfig.h"
#include "EventWatcherEpoll.h"

EventWatcherEpoll::EventWatcherEpoll(boost::function<void(int,void*)> _onRead,
				boost::function<void(int,void*)> _onWrite,
				boost::function<void(int,void*)> _onError,
				boost::function<void(int,void*)> _onAccept):
		m_onRead(_onRead),
		m_onWrite(_onWrite),
		m_onError(_onError),
		m_onAccept(_onAccept) {
	
	m_epoll = epoll_create(0x10001);
}

void EventWatcherEpoll::addSocketAccept(int _sock_fd, void *_opaque_info) {
	
	m_sockets_accept[_sock_fd] = true;
	epoll_event ev;
	ev.events = EPOLLET | EPOLLIN;// | EPOLLOUT;
	ev.data.fd = _sock_fd;
	if (epoll_ctl(m_epoll, EPOLL_CTL_ADD, _sock_fd, &ev) == -1) {
		std::cout << "EventWatcherEpoll::addSocket epoll_ctl(..) == -1";
		exit (0);
	}
}

void EventWatcherEpoll::addSocketRead(int _sock_fd, void *_opaque_info) {
	
	epoll_event ev;
	ev.events = EPOLLET | EPOLLIN;// | EPOLLOUT;
	ev.data.fd = _sock_fd;
	if (epoll_ctl(m_epoll, EPOLL_CTL_ADD, _sock_fd, &ev) == -1) {
		std::cout << "EventWatcherEpoll::addSocket epoll_ctl(..) == -1";
		exit (0);
	}
}

void EventWatcherEpoll::delSocket(int _sock_fd, void *_opaque_info) {
	
	if (epoll_ctl(m_epoll, EPOLL_CTL_DEL, _sock_fd, NULL) == -1)
		if (errno != EBADF) {
			std::cout << "EventWatcherEpoll::delSocket epoll_ctl(..) == -1";
			exit(0);
		}
}

void EventWatcherEpoll::handleEvents() {
	
	epoll_event events[4097];
	int nfds = epoll_wait(m_epoll, events, 4096, 200);
	if (nfds < 0) {
		if (errno == EINTR)
			return;
		std::cout <<  "EventWatcherEpoll::handleEvents nevents < 1";
		exit(0);
	}
	
	for (int i = 0; i<nfds; i++) {
		int fd = events[i].data.fd;
		uint32_t fevent = events[i].events;
		
	
		if (fevent & EPOLLIN)
			if (m_sockets_accept.find(fd) == m_sockets_accept.end())
				m_onRead(fd, NULL);
			else
				m_onAccept(fd, NULL);
		if (fevent & EPOLLOUT)
			m_onWrite(fd, NULL);
		if (fevent & EPOLLRDHUP)
			m_onError(fd, NULL);
	}
}

#endif // __linux__
