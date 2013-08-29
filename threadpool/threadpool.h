#ifndef THREADPOOL_H
#define  THREADPOOL_H

#include <boost/shared_ptr.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <pthread.h>

class hLock
{
    pthread_mutex_t m;
public:
    hLock()
    {
        pthread_mutex_init(&m, 0);
    }
    
    void lock()
    {
        pthread_mutex_lock(&m);
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
public:
    
    hCondWaiter()
    {
        pthread_mutex_init(&lock, 0);
        pthread_cond_init(&cond, 0);
    }
    
    void waitForIt(boost::function<bool()> state)
    {
        pthread_mutex_lock(&lock);
        
        pthread_cond_destroy(&cond);
        pthread_cond_init(&cond, 0);
        
        while (!state())
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

class hThread;

typedef boost::shared_ptr<boost::lockfree::queue< boost::function<void()> > > CallBackQueue; 
typedef boost::shared_ptr<boost::lockfree::queue<hThread*> > ThreadQueue;

class hThread
{
    CallBackQueue task_queue;
    CallBackQueue local_task_queue;
    
    ThreadQueue waiting_threads;
    
    pthread_t th;
    hCondWaiter local_queue_notempty;
public:
    
    hThread(CallBackQueue task_queue, ThreadQueue waiting_threads, pthread_t &th);
    void run();
    
    bool queueNotEmpty();
    
    void addTask(boost::function<void()> f);
    void kick();
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
    void addTask(boost::function<void()> f);
    void run();
};

#endif
