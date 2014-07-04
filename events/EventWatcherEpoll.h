#if defined __linux__

#ifndef _EVENT_WATCHER_EPOLL_H_
#define _EVENT_WATCHER_EPOLL_H_

#include "hiconfig.h"

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include <sys/epoll.h>

#include "errno.h"

class EventWatcherEpoll {
	int m_epoll;
	size_t m_nsockets;
	hiaux::hashtable<int, bool> m_sockets_accept;
	boost::function<void(int,void*)> m_onRead;
	boost::function<void(int,void*)> m_onWrite;
	boost::function<void(int,void*)> m_onError;
	boost::function<void(int,void*)> m_onAccept;
public:
	EventWatcherEpoll(boost::function<void(int,void*)> _onRead,
				boost::function<void(int,void*)> _onWrite,
				boost::function<void(int,void*)> _onError,
				boost::function<void(int,void*)> _onAccept);
	
	virtual void addSocketAccept(int _sock_fd, void *_opaque_info);
	virtual void addSocketRead(int _sock_fd, void *_opaque_info);
	virtual void delSocket(int _sock_fd, void *_opaque_info);
	
	virtual void handleEvents();
};

typedef boost::shared_ptr<EventWatcherEpoll> EventWatcherEpollPtr;

#endif // _EVENT_WATCHER_EPOLL_H_

#endif // __linux__
