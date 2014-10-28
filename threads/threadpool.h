/*
 * Copyright (c) 2010-2013  Artur Gilmutdinov

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the BSD 2-Clause License

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * BSD 2-Clause License for more details.

 * You should have received a copy of the BSD 2-Clause License
 * along with this program; if not, see <http://opensource.org/licenses>.

 * See also <http://highinit.com>
*/

#ifndef THREADPOOL_H
#define  THREADPOOL_H

#include <boost/shared_ptr.hpp>
//#include <boost/lockfree/queue.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <pthread.h>
#include <queue>
#include <boost/atomic.hpp>
//#include <pthread_rwlock.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "locks.h"

#define NEWTASK1(a) (new boost::function<void()>(boost::bind(a)))
#define NEWTASK2(a, b) (new boost::function<void()>(boost::bind(a, b)))
#define NEWTASK3(a, b, c) (new boost::function<void()>(boost::bind(a, b, c)))
#define NEWTASK4(a, b, c, d) (new boost::function<void()>(boost::bind(a, b, c, d)))
#define NEWTASK5(a, b, c, d, e) (new boost::function<void()>(boost::bind(a, b, c, d, e)))

class hThread;

//boost::lockfree::queue
//typedef boost::shared_ptr<boost::lockfree::queue< boost::function<void()>* > > CallBackQueue; 
//typedef boost::shared_ptr<boost::lockfree::queue<hThread*> > ThreadQueue;

class CallBackQueue: public std::queue< boost::function<void()>* >,
		public hLock
{

};

typedef boost::shared_ptr<CallBackQueue> CallBackQueuePtr;

class ThreadQueue: public std::queue<hThread*>,
		public hLock
{

};

typedef boost::shared_ptr<ThreadQueue> ThreadQueuePtr;

/*
class CallBackQueue: public boost::shared_ptr<std::queue<boost::function<void()>*> >,
		public hLock
{

};

class ThreadQueue: public boost::shared_ptr<std::queue<hThread*> >, 
		public hLock
{

};*/

class hThread
{
	CallBackQueuePtr task_queue;
	CallBackQueuePtr local_task_queue;

	ThreadQueuePtr waiting_threads;

	pthread_t *m_th;
	bool m_running;
	boost::atomic<size_t> *m_nrunning_threads;
public:

	hCondWaiter local_queue_notempty;
	
	hThread(CallBackQueuePtr task_queue, ThreadQueuePtr waiting_threads, pthread_t *th, boost::atomic<size_t> *_nrunning_threads);
	~hThread();
	void run();

	bool queueNotEmpty();

	void addTask(boost::function<void()> *f);
	void kick();
	void kill();
	void join();
};

class hThreadPool
{
	CallBackQueuePtr task_queue;

	hLock waiting_threads_lock;
	ThreadQueuePtr waiting_threads;
	std::vector<hThread*> threads;
	size_t nthreads;
	
	boost::atomic<size_t> m_nrunning_threads;
	
	
public:

	hThreadPool(int nthreads);
	~hThreadPool();

	void addTask(boost::function<void()>* f);
	void run();
	void kill();
	void join();
};

typedef boost::shared_ptr<hThreadPool> hThreadPoolPtr;

#endif
