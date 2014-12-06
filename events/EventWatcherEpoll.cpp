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
	
	m_epoll = epoll_create(4096);
}

void EventWatcherEpoll::addSocket(int _sock_fd, uint32_t _mask, void *_opaque_info) {
	
	epoll_event ev;
	
	ev.events = EPOLLET;
	
	if (_mask & HI_READ)
		ev.events |= EPOLLIN;
	
	if (_mask & HI_WRITE)
		ev.events |= EPOLLOUT;
	
	ev.data.fd = _sock_fd;
	
	m_sockets_masks[_sock_fd] = _mask;
	
	if (epoll_ctl(m_epoll, EPOLL_CTL_ADD, _sock_fd, &ev) == -1) {
		std::cout << "EventWatcherEpoll::addSocket epoll_ctl(..) == -1";
		exit (0);
	}
}

void EventWatcherEpoll::enableEvents(int _sock_fd, uint32_t _mask) {
	
	hiaux::hashtable<int, uint32_t>::iterator it = m_sockets_masks.find(_sock_fd);
	
	if (it == m_sockets_masks.end()) {
		
		return;
	}
	
	uint32_t mask = it->second;
	uint32_t need_enable = (~mask) & _mask;
	uint32_t epoll_mask = 0;
	
	if (need_enable & HI_READ)
		epoll_mask |= EPOLLIN;
	
	if (need_enable & HI_WRITE)
		epoll_mask |= EPOLLOUT;
	
	m_sockets_masks[_sock_fd] = mask | need_enable;
	
	epoll_event ev;
	ev.events = epoll_mask;
	ev.data.fd = _sock;

	if (epoll_ctl(m_epoll, EPOLL_CTL_MOD, _sock_fd, &ev) == -1) {
		std::cout << "EventWatcherEpoll::addSocket epoll_ctl(..) == -1";
		exit (0);
	}
}

void EventWatcherEpoll::delSocket(int _sock_fd, void *_opaque_info) {
	
	hiaux::hashtable<int, uint32_t>::iterator it = m_sockets_masks.find(_sock_fd);
	
	if (it == m_sockets_masks.end()) {
		
		return;
	}

	if (epoll_ctl(m_epoll, EPOLL_CTL_DEL, _sock_fd, NULL) == -1)
		if (errno != EBADF) {
			std::cout << "EventWatcherEpoll::delSocket epoll_ctl(..) == -1";
			exit(0);
		}
	
	m_sockets_masks.erase(it);
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
		
		hiaux::hashtable<int, uint32_t>::iterator it = m_sockets_masks.find(event.ident);
	
		if (fevent & EPOLLIN)
			if (it == m_sockets_masks.end())
				m_onRead(fd, NULL);
			else {
				if (it->second & HI_ACCEPT)
					m_onAccept(fd, NULL);
			}
		
		if (fevent & EPOLLOUT)
			m_onWrite(fd, NULL);
		
		if (fevent & EPOLLRDHUP)
			m_onError(fd, NULL);
	}
}

#endif // __linux__
