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

hThread::hThread(CallBackQueuePtr task_queue, ThreadQueuePtr waiting_threads, pthread_t *th,
			boost::atomic<size_t> *_nrunning_threads):
	local_queue_notempty(boost::bind(&hThread::queueNotEmpty, this)),
	m_nrunning_threads(_nrunning_threads),
	m_running(true) {
	this->waiting_threads = waiting_threads;
	this->local_task_queue = CallBackQueuePtr (new CallBackQueue);//new boost::lockfree::queue< boost::function<void()>* >(100) );
	this->task_queue = task_queue;
	m_th = th;
}

hThread::~hThread() {
	
	//std::cout << "hThread::~hThread\n";
	
	pthread_detach(*m_th);
	delete m_th;
}

void _thread_signal_callback_handler(int signum) {
	
	pid_t pid;
	int status;
	while ( (pid = waitpid(-1, &status, WNOHANG)) > 0);
}

void hThread::run() {
	
	sigset_t set;
	sigfillset(&set);
	pthread_sigmask(SIG_SETMASK, &set, NULL);
	
	signal(SIGCHLD, _thread_signal_callback_handler);
	
	m_nrunning_threads->fetch_add(1);
	boost::function<void()> *f;
	while (m_running)
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
		
		if (!m_running)
			break;
		
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
		
		if (!m_running)
			break;
		
		waiting_threads->lock();
		waiting_threads->push(this);
		waiting_threads->unlock();
		local_queue_notempty.wait();     
	}
	m_nrunning_threads->fetch_sub(1);
	//std::cout << "thread stop" << std::endl;
}

bool hThread::queueNotEmpty() {
	local_task_queue->lock();
	bool notempty = !local_task_queue->empty();
	local_task_queue->unlock();
	return notempty;
}

void hThread::addTask(boost::function<void()> *f) {
	local_task_queue->lock();
	local_task_queue->push(f);
	local_task_queue->unlock();
}

void hThread::kick() {
	local_queue_notempty.kick();
}

void hThread::kill() {
	m_running = false;
}

void hThread::join() {
	void *end;
	pthread_join(*m_th, &end);
}

hThreadPool::hThreadPool(int nthreads) {
	m_nrunning_threads = 0;
	this->nthreads = nthreads;
	this->task_queue = CallBackQueuePtr (new CallBackQueue);//new boost::lockfree::queue< boost::function<void()>* >(100000));
	this->waiting_threads = ThreadQueuePtr (new ThreadQueue);//new boost::lockfree::queue<hThread*>(100000));
}

void hThreadPool::addTask(boost::function<void()> *f) {
	waiting_threads->lock();
	if (!waiting_threads->empty()) {
		hThread *thread = waiting_threads->front();
		waiting_threads->pop();
		waiting_threads->unlock();
		
		thread->local_queue_notempty.lock();
		thread->addTask(f);
		thread->kick();
		thread->local_queue_notempty.unlock();
	}
	else {
		waiting_threads->unlock();
		task_queue->lock();
		task_queue->push(f);
		task_queue->unlock();
	}
}

void *call_boost_function(void *a) {
	boost::function<void()> *f = (boost::function<void()> *)a;
	(*f)();
	delete f;
	return 0;
}

void hThreadPool::run() {
	for (int i = 0; i<nthreads; i++) {
		pthread_t *th = new pthread_t;
		hThread *thread = new hThread(task_queue, waiting_threads, th, &m_nrunning_threads);
		threads.push_back(thread);
		boost::function<void()> *f  = new boost::function<void()>;
		*f = boost::bind(&hThread::run, thread);
		pthread_create(th, 0, &call_boost_function, (void*)f);
	}
}

void hThreadPool::kill() {
	//std::cout << "hThreadPool::kill\n";
	//std::cout << "running threads " << m_nrunning_threads.load();
	
	for (int i = 0; i<threads.size(); i++) {
		threads[i]->kill();
	}
	
	waiting_threads->lock();
	while (!waiting_threads->empty()) {
		hThread *thread = waiting_threads->front();
		waiting_threads->pop();
		waiting_threads->unlock();
		
		thread->addTask( NEWTASK2 (&hThread::kill, thread) );
		
		thread->kick();
		waiting_threads->lock();
	}
	waiting_threads->unlock();
}

void hThreadPool::join() {
	
	for (int i = 0; i<threads.size(); i++) {
		threads[i]->join();
	}
}

hThreadPool::~hThreadPool() {
//	std::cout << "hThreadPool::~hThreadPool\n";
//	std::cout << "running threads " << m_nrunning_threads.load();
	//kill();
	while (m_nrunning_threads.load() > 0) {
		kill();
	}
///	std::cout << "all threads shut down\n";
	for (int i = 0; i<threads.size(); i++)
		delete threads[i];
}
