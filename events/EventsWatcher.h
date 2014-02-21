#ifndef _HIAUX_EVENTS_H_
#define _HIAUX_EVENTS_H_

#include "hiconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/event.h>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

class EventWatcher {	
	int m_kqueue;
	boost::function<void(int,void*)> m_onRead;
	boost::function<void(int,void*)> m_onWrite;
	boost::function<void(int,void*)> m_onError;
public:
	EventWatcher(boost::function<void(int,void*)> _onRead,
				boost::function<void(int,void*)> _onWrite,
				boost::function<void(int,void*)> _onError);
	void addSocket(int _sock_fd, void *_opaque_info);
	void delSocket(int _sock_fd, void *_opaque_info);
	void handleEvents();
};

typedef boost::shared_ptr<EventWatcher> EventWatcherPtr;

#endif // _HIAUX_EVENTS_H_
