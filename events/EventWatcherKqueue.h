
#if defined (__APPLE__) || defined (__OpenBSD__) || defined (__NetBSD__) || defined (__FreeBSD__) || defined (__DragonflyBSD__)

#ifndef _EVENT_WATCHER_KQUEUE_H_
#define _EVENT_WATCHER_KQUEUE_H_

#include "hiconfig.h"
#include "hiaux/structs/hashtable.h"
#include "hiaux/threads/locks.h"
//#include "EventsWatcher.h"

//#ifdef _EVENT_WATCHER_IS_KQUEUE_

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/event.h>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

class EventWatcherKqueue {	
	int m_kqueue;
	size_t m_nsockets;
	hiaux::hashtable<int, bool> m_sockets_accept;
	boost::function<void(int,void*)> m_onRead;
	boost::function<void(int,void*)> m_onWrite;
	boost::function<void(int,void*)> m_onError;
	boost::function<void(int,void*)> m_onAccept;
//	hAutoLockPtr m_lock;
public:
	EventWatcherKqueue(boost::function<void(int,void*)> _onRead,
				boost::function<void(int,void*)> _onWrite,
				boost::function<void(int,void*)> _onError,
				boost::function<void(int,void*)> m_onAccept);
	~EventWatcherKqueue();
	virtual void addSocketAccept(int _sock_fd, void *_opaque_info);
	virtual void addSocketRead(int _sock_fd, void *_opaque_info);
	virtual void delSocket(int _sock_fd, void *_opaque_info);
	virtual void handleEvents();
};

typedef boost::shared_ptr<EventWatcherKqueue> EventWatcherKqueuePtr;

//#endif // _EVENT_WATCHER_IS_KQUEUE_

#endif // _EVENT_WATCHER_KQUEUE_H_

#endif