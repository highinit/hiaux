
#include "hiconfig.h"
#include "EventsWatcher.h"

EventWatcher::EventWatcher(boost::function<void(int,void*)> _onRead,
				boost::function<void(int,void*)> _onWrite,
				boost::function<void(int,void*)> _onError):
		m_onRead(_onRead),
		m_onWrite(_onWrite),
		m_onError(_onError)
{
	m_kqueue = kqueue();
}

void EventWatcher::addSocket(int _sock_fd, void *_opaque_info)
{
	struct kevent ev;
	EV_SET(&ev, _sock_fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, _opaque_info);
	if (kevent(m_kqueue, &ev, 1, NULL, 0, NULL) == -1 ) {
		std::cout << "EventWatcher::addEvent kevent(m_kqueue, &ev, 1, NULL, 0, NULL) == -1";
		exit (0);
	}
	
}

void EventWatcher::delSocket(int _sock_fd, void *_opaque_info)
{
	//std::cout << "EventWatcher::delSocket\n";
	struct kevent ev;
	EV_SET(&ev, _sock_fd, EVFILT_READ, EV_DELETE, 0, 0, _opaque_info);
	if (kevent(m_kqueue, &ev, 1, NULL, 0, NULL) == -1 ) {
		std::cout << "EventWatcher::delSocket kevent(m_kqueue, &ev, 1, NULL, 0, NULL) == -1";
		exit(0);
	}
}

void EventWatcher::handleEvents()
{
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
		else if (event.filter == EVFILT_READ)
			m_onRead(event.ident, event.udata);
	}
}
