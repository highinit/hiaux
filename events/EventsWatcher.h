#ifndef _HIAUX_EVENTS_H_
#define _HIAUX_EVENTS_H_

#include "hiconfig.h"

#if defined __linux__
#define _EVENT_WATCHER_IS_EPOLL_
#define EventWatcher EventWatcherEpoll
#define EventWatcherPtr EventWatcherEpollPtr
#include "EventWatcherEpoll.h"

#define EventWatcher EventWatcherEpoll
#define EventWatcherPtr EventWatcherEpollPtr

#elif defined (__APPLE__) || defined (__OpenBSD__) || defined (__NetBSD__) || defined (__FreeBSD__) || defined (__DragonflyBSD__)
#include "EventWatcherKqueue.h"

#define EventWatcher EventWatcherKqueue
#define EventWatcherPtr EventWatcherKqueuePtr

#else
#error "Unknown platofrm. Don't know what to use epoll or kqueue" 

#endif

#endif // _HIAUX_EVENTS_H_
