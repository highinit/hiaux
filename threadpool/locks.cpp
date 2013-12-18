#include <boost/smart_ptr/shared_ptr.hpp>

#include "locks.h"

hLockTicket::hLockTicket(pmutexPtr lock)
{
	m_lock = lock;
	pthread_mutex_lock(m_lock.get());
}

hLockTicket::hLockTicket(const hLockTicket &a)
{
	m_locked = 1;
	m_lock = a.m_lock; 
}

hLockTicket::~hLockTicket()
{
	if (m_locked.load())
		pthread_mutex_unlock(m_lock.get());
}

void hLockTicket::unlock()
{
	if (m_locked.load())
	{
		pthread_mutex_unlock(m_lock.get());
		m_locked = 0;
	}
}

hAutoLock::hAutoLock()
{
	m_lock.reset(new pmutex);
}

hAutoLock::~hAutoLock()
{
}

hLockTicket hAutoLock::lock()
{
	return hLockTicket(m_lock);
}

hLock& hLock::operator=(const hLock &a)
{ 
	return *this;
}

hLock::hLock()
{
	pthread_mutex_init(&m, 0);
}

hLock::~hLock()
{

}

void hLock::lock()
{
	pthread_mutex_lock(&m);
}

bool hLock::trylock()
{
	return pthread_mutex_trylock(&m) == 0;
}

void hLock::unlock()
{
	pthread_mutex_unlock(&m);
}

hCondWaiter::hCondWaiter(boost::function<bool()> stateReachedFunc)
{
	pthread_mutex_init(&m_lock, 0);
	pthread_cond_init(&cond, 0);
	m_state_ok = stateReachedFunc;
}

void hCondWaiter::wait()
{
	pthread_mutex_lock(&m_lock);

	while (!m_state_ok())
	{
		pthread_cond_wait(&cond, &m_lock);
	}

	pthread_cond_destroy(&cond);
	pthread_cond_init(&cond, 0);

	pthread_mutex_unlock(&m_lock);
}

void hCondWaiter::lock()
{
	pthread_mutex_lock(&m_lock);
}

void hCondWaiter::unlock()
{
	pthread_mutex_unlock(&m_lock);
}

void hCondWaiter::kick()
{
	pthread_cond_signal(&cond);
}

hRWLockRead::hRWLockRead(pthread_rwlock_t *lock)
{
	m_lock = lock;
	pthread_rwlock_rdlock(m_lock);
}

hRWLockRead::~hRWLockRead()
{
	pthread_rwlock_unlock(m_lock);
}

hRWLockRead::hRWLockRead(const hRWLockRead &a)
{
	this->m_lock = a.m_lock;
}

void hRWLockRead::unlock()
{
	pthread_rwlock_unlock(m_lock);
}

hRWLockWrite::hRWLockWrite(pthread_rwlock_t *lock)
{
	m_lock = lock;
	m_locked = 1;
	pthread_rwlock_wrlock(m_lock);
}

hRWLockWrite::~hRWLockWrite()
{
	if (m_locked.load())
	pthread_rwlock_unlock(m_lock);
}

hRWLockWrite::hRWLockWrite(const hRWLockWrite &a)
{
	m_lock = a.m_lock;
	m_locked = a.m_locked.load();
}

void hRWLockWrite::unlock()
{
	m_locked = 0;
	pthread_rwlock_unlock(m_lock);
}

hRWLock::hRWLock()
{
	m_lock = new pthread_rwlock_t;
	pthread_rwlock_init(m_lock, 0);
}

hRWLock::~hRWLock()
{
	delete m_lock;
}

hRWLockRead hRWLock::read()
{
	return hRWLockRead(m_lock);
}

hRWLockWrite hRWLock::write()
{
	return hRWLockWrite(m_lock);
}