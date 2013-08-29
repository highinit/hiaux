#include "threadpool.h"

hThread::hThread(CallBackQueue task_queue, ThreadQueue waiting_threads, pthread_t &th)
{
    this->waiting_threads = waiting_threads;
    this->local_task_queue = CallBackQueue (new boost::lockfree::queue< boost::function<void()> >(100));
    this->task_queue = task_queue;
    this->th = th;
}

void hThread::run()
{
    boost::function<void()> f;
    while (1)
    {
        // local
        while (local_task_queue->pop(f))
        {
            f();
        }
        
        // global
        while (task_queue->pop(f))
        {
            f();
        }    
        
        waiting_threads->push(this);
        local_queue_notempty.waitForIt(boost::bind(&hThread::queueNotEmpty, this));     
    }
}

bool hThread::queueNotEmpty()
{
    return !local_task_queue->empty();
}

void hThread::addTask(boost::function<void()> f)
{
    local_task_queue->push(f);
}

void hThread::kick()
{
    local_queue_notempty.kick();
}

hThreadPool::hThreadPool(int nthreads)
{
    this->nthreads = nthreads;
    this->task_queue = CallBackQueue (new boost::lockfree::queue< boost::function<void()> >(100));
    this->waiting_threads = ThreadQueue (new boost::lockfree::queue<hThread*>(100));
}

void hThreadPool::addTask(boost::function<void()> f)
{
    hThread *thread;

    if (waiting_threads->pop(thread))
    {
        thread->addTask(f);
        thread->kick();
    }
    else
    {
        task_queue->push(f);
    }  
}

void *call_boost_function(void *a)
{
    boost::function<void()> *f = (boost::function<void()> *)a;
    (*f)();
}

void hThreadPool::run()
{
    for (int i = 0; i<nthreads; i++)
    {
        pthread_t th;
        hThread *thread = new hThread(task_queue, waiting_threads, th);
        threads.push_back(thread);
        boost::function<void()> *f  = new boost::function<void()>;
        *f = boost::bind(&hThread::run, thread);
        pthread_create(&th, 0, &call_boost_function, (void*)f);
    }
}