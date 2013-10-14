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

#include "threadpool.h"

hThread::hThread(CallBackQueue task_queue, ThreadQueue waiting_threads, pthread_t *th):
	local_queue_notempty(boost::bind(&hThread::queueNotEmpty, this))
{
    this->waiting_threads = waiting_threads;
    this->local_task_queue = CallBackQueue (new boost::lockfree::queue< boost::function<void()>* >(100) );
    this->task_queue = task_queue;
	m_th = th;
}

void hThread::run()
{
    boost::function<void()> *f;
    while (1)
    {
        // local
        while (local_task_queue->pop(f))
        {
            (*f)();
            delete f;
        }
        
        // global
        while (task_queue->pop(f))
        {
            (*f)();
            delete f;
        }    
        
        waiting_threads->push(this);
        local_queue_notempty.wait();     
    }
}

bool hThread::queueNotEmpty()
{
    return !local_task_queue->empty();
}

void hThread::addTask(boost::function<void()> *f)
{
    local_task_queue->push(f);
}

void hThread::kick()
{
    local_queue_notempty.kick();
}

void hThread::join()
{
	void *end;
    pthread_join(*m_th, &end);
}

hThreadPool::hThreadPool(int nthreads)
{
    this->nthreads = nthreads;
    this->task_queue = CallBackQueue (new boost::lockfree::queue< boost::function<void()>* >(10000));
    this->waiting_threads = ThreadQueue (new boost::lockfree::queue<hThread*>(10000));
}

void hThreadPool::addTask(boost::function<void()> *f)
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
    delete f;
    return 0;
}

void hThreadPool::run()
{
    for (int i = 0; i<nthreads; i++)
    {
        pthread_t *th = new pthread_t;
        hThread *thread = new hThread(task_queue, waiting_threads, th);
        threads.push_back(thread);
        boost::function<void()> *f  = new boost::function<void()>;
        *f = boost::bind(&hThread::run, thread);
        pthread_create(th, 0, &call_boost_function, (void*)f);
    }
}

void hThreadPool::join()
{
    for (int i = 0; i<threads.size(); i++)
    {
		threads[i]->join();
    }
}
