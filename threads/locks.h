/* 
 * File:   locks.h
 * Author: phrk
 *
 * Created on December 18, 2013, 11:10 PM
 */

#ifndef LOCKS_H
#define	LOCKS_H

#include "hiconfig.h"

#include <pthread.h>
#include <boost/atomic.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

typedef pthread_mutex_t pmutex;
typedef boost::shared_ptr<pthread_mutex_t> pmutexPtr;

class hAutoLock;

class hLockTicket : public boost::noncopyable
{
public:

	pmutexPtr m_lock;
	boost::atomic<int> m_locked;
	
//	friend hAutoLock;
	
	hLockTicket(pmutexPtr lock, bool locked);
	//hLockTicket(const hLockTicket &a);
	~hLockTicket();
	void unlock();
};

typedef boost::shared_ptr<hLockTicket> hLockTicketPtr;

class hAutoLock
{
	pmutexPtr m_lock;
	//hAutoLock(const hAutoLock&);
	//hAutoLock& operator=(const hAutoLock &a);
public:
	hAutoLock();
	~hAutoLock();
	hLockTicketPtr lock();
	hLockTicketPtr tryLock();
};

class hLock
{
	pthread_mutex_t m;
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

typedef boost::shared_ptr<hCondWaiter> hCondWaiterPtr;

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

