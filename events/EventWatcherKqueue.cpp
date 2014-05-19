
//#include "hiconfig.h"

//#ifdef _EVENT_WATCHER_IS_KQUEUE_

#if defined (__APPLE__) || defined (__OpenBSD__) || defined (__NetBSD__) || defined (__FreeBSD__) || defined (__DragonflyBSD__)

#include "EventWatcherKqueue.h"
//#include "EventsWatcher.h"

EventWatcherKqueue::EventWatcherKqueue(boost::function<void(int,void*)> _onRead,
				boost::function<void(int,void*)> _onWrite,
				boost::function<void(int,void*)> _onError,
				boost::function<void(int,void*)> _onAccept):
		m_onRead(_onRead),
		m_onWrite(_onWrite),
		m_onError(_onError),
		m_onAccept(_onAccept)
{
	m_nsockets = 0;
	m_kqueue = kqueue();
}

EventWatcherKqueue::~EventWatcherKqueue() {
//	std::cout << "EventWatcherKqueue::~EventWatcherKqueue\n";
}

void EventWatcherKqueue::addSocketAccept(int _sock_fd, void *_opaque_info) {
	
	//hLockTicketPtr ticket = m_lock->lock();
	
	m_sockets_accept[_sock_fd] = true;
	struct kevent ev;
	struct timespec timeout;
	timeout.tv_nsec = 0;
	timeout.tv_sec = 10;
	EV_SET(&ev, _sock_fd, EVFILT_READ, EV_ADD, 0, 0, _opaque_info);
	if (kevent(m_kqueue, &ev, 1, NULL, 0, &timeout) == -1 ) {
		std::cout << "EventWatcher::addEvent kevent(m_kqueue, &ev, 1, NULL, 0, NULL) == -1";
		exit (0);
	}
}

void EventWatcherKqueue::addSocketRead(int _sock_fd, void *_opaque_info) {
	//m_nsockets++;
	//hLockTicketPtr ticket = m_lock->lock();
	
	struct kevent ev;
	struct timespec timeout;
	timeout.tv_nsec = 0;
	timeout.tv_sec = 10;
	EV_SET(&ev, _sock_fd, EVFILT_READ, EV_ADD, 0, 0, _opaque_info);
	if (kevent(m_kqueue, &ev, 1, NULL, 0, &timeout) == -1 ) {
		std::cout << "EventWatcher::addEvent kevent(m_kqueue, &ev, 1, NULL, 0, NULL) == -1";
		exit (0);
	}
}

void EventWatcherKqueue::delSocket(int _sock_fd, void *_opaque_info) {
	//m_nsockets--;
	//std::cout << "EventWatcher::delSocket\n";
	
	//hLockTicketPtr ticket = m_lock->lock();
	
	struct kevent ev;
	EV_SET(&ev, _sock_fd, EVFILT_READ, EV_DELETE, 0, 0, _opaque_info);
	if (kevent(m_kqueue, &ev, 1, NULL, 0, NULL) == -1 ) {
		std::cout << "EventWatcher::delSocket kevent(m_kqueue, &ev, 1, NULL, 0, NULL) == -1";
		exit(0);
	}
}

void EventWatcherKqueue::handleEvents() {
	//std::cout << "m_nsockets: " << m_nsockets << std::endl;
	
	//hLockTicketPtr ticket = m_lock->lock();
	
	struct kevent events[4096];
	timespec timeout = {0, 250000000};
	int nevents = kevent(m_kqueue, NULL, 0, events, 4096, &timeout);
	
	if (nevents < 1) {
		//if (errno == EINTR)
			return;
		//std::cout <<  "EventWatcher::handleEvents nevents < 1";
		//exit(0);
	}
	
	for (int i = 0; i<nevents; i++) {
		struct kevent &event = events[i];
		if (event.flags == EV_ERROR)
			m_onError(event.ident, event.udata);
		else if (event.filter == EVFILT_WRITE)
			m_onWrite(event.ident, event.udata);
		else if (event.filter == EVFILT_READ) {
			if (m_sockets_accept.find(event.ident) == m_sockets_accept.end())
				m_onRead(event.ident, event.udata);
			else
				m_onAccept(event.ident, event.udata);
		}
	}
}

//#endif // _EVENT_WATCHER_IS_KQUEUE_
#endif
