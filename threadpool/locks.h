/* 
 * File:   locks.h
 * Author: phrk
 *
 * Created on December 18, 2013, 11:10 PM
 */

#ifndef LOCKS_H
#define	LOCKS_H

#include <pthread.h>
#include <boost/atomic.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

typedef pthread_mutex_t pmutex;
typedef boost::shared_ptr<pthread_mutex_t> pmutexPtr;

class hLockTicket
{
	pmutexPtr m_lock;
	boost::atomic<int> m_locked;
public:
	hLockTicket(pmutexPtr lock);
	hLockTicket(const hLockTicket &a);
	~hLockTicket();
	void unlock();
};

class hAutoLock
{
	pmutexPtr m_lock;
public:
	hAutoLock();
	~hAutoLock();
	hLockTicket lock();
};

class hLock
{
	pthread_mutex_t m;
	hLock(const hLock&);
	hLock &operator=(const hLock &);
public:
	
	hLock();
	~hLock();
	void lock();
	void unlock();
	bool trylock();
	
};

typedef boost::shared_ptr<hAutoLock> hAutoLockPtr;

class hCondWaiter
{
	pthread_mutex_t m_lock;
	pthread_cond_t cond;
	boost::function<bool()> m_state_ok;
	
	hCondWaiter();
	hCondWaiter(hCondWaiter&);
	hCondWaiter& operator=(hCondWaiter&);
	
public:

	hCondWaiter(boost::function<bool()> stateReachedFunc);

	void wait();
	void lock();
	void unlock();
	void kick();
};

class hRWLockRead
{
	pthread_rwlock_t *m_lock;
public:

	hRWLockRead(pthread_rwlock_t *lock);
	hRWLockRead(const hRWLockRead &a);
	~hRWLockRead();
	void unlock();
};

class hRWLockWrite
{
	pthread_rwlock_t *m_lock;
	boost::atomic<int> m_locked;
public:

	hRWLockWrite(pthread_rwlock_t *lock);
	hRWLockWrite(const hRWLockWrite &a);
	~hRWLockWrite();
	
	void unlock();
};

class hRWLock
{
	pthread_rwlock_t *m_lock;
public:

	hRWLock();
	~hRWLock();

	hRWLockRead read();
	hRWLockWrite write();
};

#endif	/* LOCKS_H */

