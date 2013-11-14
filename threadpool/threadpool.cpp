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

hThread::hThread(CallBackQueuePtr task_queue, ThreadQueuePtr waiting_threads, pthread_t *th):
	local_queue_notempty(boost::bind(&hThread::queueNotEmpty, this))
{
	this->waiting_threads = waiting_threads;
	this->local_task_queue = CallBackQueuePtr (new CallBackQueue);//new boost::lockfree::queue< boost::function<void()>* >(100) );
	this->task_queue = task_queue;
	m_th = th;
}

void hThread::run()
{
	boost::function<void()> *f;
	while (1)
	{
		// local
		local_task_queue->lock();
		while (!local_task_queue->empty())
		{
			f = local_task_queue->front();
			local_task_queue->pop();
			local_task_queue->unlock();
			
			(*f)();
			delete f;
			local_task_queue->lock();
		}
		local_task_queue->unlock();
		
		// global
		task_queue->lock();
		while (!task_queue->empty())
		{
			f = task_queue->front();
			task_queue->pop();
			task_queue->unlock();
			
			(*f)();
			delete f;
			task_queue->lock();
		}
		task_queue->unlock();
		
		waiting_threads->lock();
		waiting_threads->push(this);
		waiting_threads->unlock();
		local_queue_notempty.wait();     
	}
}

bool hThread::queueNotEmpty()
{
	local_task_queue->lock();
	bool notempty = !local_task_queue->empty();
	local_task_queue->unlock();
	return notempty;
}

void hThread::addTask(boost::function<void()> *f)
{
	local_task_queue->lock();
	local_task_queue->push(f);
	local_task_queue->unlock();
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
	this->task_queue = CallBackQueuePtr (new CallBackQueue);//new boost::lockfree::queue< boost::function<void()>* >(100000));
	this->waiting_threads = ThreadQueuePtr (new ThreadQueue);//new boost::lockfree::queue<hThread*>(100000));
}

void hThreadPool::addTask(boost::function<void()> *f)
{
	hThread *thread;

	waiting_threads->lock();
	if (!waiting_threads->empty())
	{
		hThread *thread = waiting_threads->front();
		waiting_threads->pop();
		waiting_threads->unlock();
		
		thread->local_queue_notempty.lock();
		thread->addTask(f);
		thread->kick();
		thread->local_queue_notempty.unlock();
	}
	else
	{
		waiting_threads->unlock();
		task_queue->lock();
		task_queue->push(f);
		task_queue->unlock();
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
