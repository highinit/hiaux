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
#include <boost/lockfree/queue.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <pthread.h>
#include <queue>
#include <atomic>
//#include <pthread_rwlock.h>

class hLock
{
    pthread_mutex_t m;
    
    hLock &operator=(const hLock &a)
    { 
		return *this;
	}
    
public:
    hLock()
    {
        pthread_mutex_init(&m, 0);
    }
    
    void lock()
    {
        pthread_mutex_lock(&m);
    }
    
    bool trylock()
    {
        return pthread_mutex_trylock(&m) == 0;
    }
    
    void unlock()
    {
        pthread_mutex_unlock(&m);
    }
};

class hCondWaiter
{
    pthread_mutex_t lock;
    pthread_cond_t cond;
	boost::function<bool()> m_state_ok;
public:

	hCondWaiter(boost::function<bool()> stateReachedFunc)
	{
		pthread_mutex_init(&lock, 0);
		pthread_cond_init(&cond, 0);
		m_state_ok = stateReachedFunc;
	}

    void wait()
    {
		pthread_mutex_lock(&lock);

	/*	if (m_state_ok())
		{
			pthread_mutex_unlock(&lock);
			return;
		}
*/
		pthread_cond_destroy(&cond);
		pthread_cond_init(&cond, 0);

		while (!m_state_ok())
		{
			pthread_cond_wait(&cond, &lock);
		}
		pthread_mutex_unlock(&lock);
    }
    
    void kick()
    {
        pthread_mutex_lock(&lock);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&lock);
    }
};

class hRWLockRead
{
    pthread_rwlock_t *m_lock;
public:
    
    hRWLockRead(pthread_rwlock_t *lock)
    {
        m_lock = lock;
        pthread_rwlock_rdlock(m_lock);
    }
    
    ~hRWLockRead()
    {
        pthread_rwlock_unlock(m_lock);
    }
    
    hRWLockRead(const hRWLockRead &a)
    {
        this->m_lock = a.m_lock;
    }
    
    void unlock()
    {
        pthread_rwlock_unlock(m_lock);
    }
};

class hRWLockWrite
{
    pthread_rwlock_t *m_lock;
    std::atomic<int> m_locked;
public:
    
    hRWLockWrite(pthread_rwlock_t *lock)
    {
	m_lock = lock;
	m_locked = 1;
	pthread_rwlock_wrlock(m_lock);
    }

    ~hRWLockWrite()
    {
	if (m_locked.load())
	pthread_rwlock_unlock(m_lock);
    }

    hRWLockWrite(const hRWLockWrite &a)
    {
	m_lock = a.m_lock;
	m_locked = a.m_locked.load();
    }

    void unlock()
    {
	m_locked = 0;
	pthread_rwlock_unlock(m_lock);
    }
};

class hRWLock
{
    pthread_rwlock_t *m_lock;
public:
    
    hRWLock()
    {
        m_lock = new pthread_rwlock_t;
        pthread_rwlock_init(m_lock, 0);
    }
    
    ~hRWLock()
    {
        delete m_lock;
    }
    
    hRWLockRead read()
    {
        return hRWLockRead(m_lock);
    }
    
    hRWLockWrite write()
    {
        return hRWLockWrite(m_lock);
    }
    
};

class hThread;

//boost::lockfree::queue
typedef boost::shared_ptr<boost::lockfree::queue< boost::function<void()>* > > CallBackQueue; 
typedef boost::shared_ptr<boost::lockfree::queue<hThread*> > ThreadQueue;

//typedef boost::shared_ptr<std::queue< boost::function<void()> > > CallBackQueue; 
//typedef boost::shared_ptr<std::queue<hThread*> > ThreadQueue;

class hThread
{
    CallBackQueue task_queue;
    CallBackQueue local_task_queue;
    
    ThreadQueue waiting_threads;
    
    pthread_t *m_th;
    hCondWaiter local_queue_notempty;
public:
    
    hThread(CallBackQueue task_queue, ThreadQueue waiting_threads, pthread_t *th);
    void run();
    
    bool queueNotEmpty();
    
    void addTask(boost::function<void()> *f);
    void kick();
    void join();
};

class hThreadPool
{
    CallBackQueue task_queue;
    
    hLock waiting_threads_lock;
    ThreadQueue waiting_threads;
    std::vector<hThread*> threads;
    size_t nthreads;
public:
    
    hThreadPool(int nthreads);
    // boost::function<void()> f = boost::bind(&hTaskHandler::calc, &handler);
    void addTask(boost::function<void()>* f);
    void run();
    void join();
};

#endif
