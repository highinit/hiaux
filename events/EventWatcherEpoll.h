#if defined __linux__

#ifndef _EVENT_WATCHER_EPOLL_H_
#define _EVENT_WATCHER_EPOLL_H_

#include "hiconfig.h"

#include "hiaux/structs/hashtable.h"

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <iostream>

#include <sys/epoll.h>

#include "errno.h"

#define	HI_ACCEPT (1<<0)
#define	HI_READ (1<<2)
#define	HI_WRITE (1<<3)

class EventWatcherEpoll : public boost::noncopyable {
	int m_epoll;
	size_t m_nsockets;
	
	hiaux::hashtable<int, uint32_t> m_sockets_masks;
	
	boost::function<void(int,void*)> m_onRead;
	boost::function<void(int,void*)> m_onWrite;
	boost::function<void(int,void*)> m_onError;
	boost::function<void(int,void*)> m_onAccept;
public:
	EventWatcherEpoll(boost::function<void(int,void*)> _onRead,
				boost::function<void(int,void*)> _onWrite,
				boost::function<void(int,void*)> _onError,
				boost::function<void(int,void*)> _onAccept);
	
	void addSocket(int _sock_fd, uint32_t _mask, void *_opaque_info);
	void enableEvents(int _sock_fd, uint32_t _mask);
	virtual void delSocket(int _sock_fd);
	
//	virtual void addSocketAccept(int _sock_fd, void *_opaque_info);
//	virtual void addSocketRead(int _sock_fd, void *_opaque_info);
//	virtual void delSocket(int _sock_fd, void *_opaque_info);
	
	virtual void handleEvents();
};

typedef boost::shared_ptr<EventWatcherEpoll> EventWatcherEpollPtr;

#endif // _EVENT_WATCHER_EPOLL_H_

#endif // __linux__
