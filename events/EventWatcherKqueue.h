
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
#include <boost/noncopyable.hpp>

#define	HI_ACCEPT (1<<0)
#define	HI_READ (1<<2)
#define	HI_WRITE (1<<3)

class CannotCreateEventWatcherEx {
};

class EventWatcherKqueue :public boost::noncopyable {	
	int m_kqueue;
	size_t m_nsockets;
	
	hiaux::hashtable<int, uint32_t> m_sockets_masks;
	
	boost::function<void(int,void*)> m_onRead;
	boost::function<void(int,void*)> m_onWrite;
	boost::function<void(int,void*)> m_onError;
	boost::function<void(int,void*)> m_onAccept;
	hAutoLock m_lock;
public:
	EventWatcherKqueue(boost::function<void(int,void*)> _onRead,
				boost::function<void(int,void*)> _onWrite,
				boost::function<void(int,void*)> _onError,
				boost::function<void(int,void*)> _onAccept);
	~EventWatcherKqueue();
	
	void addSocket(int _sock_fd, uint32_t _mask, void *_opaque_info);
	void enableEvents(int _sock_fd, uint32_t _mask);
	virtual void delSocket(int _sock_fd);
	
	//virtual void addSocketAccept(int _sock_fd, void *_opaque_info);
	//virtual void addSocketRead(int _sock_fd, void *_opaque_info);
	
	virtual void handleEvents();
};

typedef boost::shared_ptr<EventWatcherKqueue> EventWatcherKqueuePtr;

//#endif // _EVENT_WATCHER_IS_KQUEUE_

#endif // _EVENT_WATCHER_KQUEUE_H_

#endif