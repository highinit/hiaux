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

#include "ReduceDispatcher.h"

#include <sys/types.h>
#include <dirent.h>

#include <sys/uio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/uio.h> 
#include <unistd.h>

#include "../example_invindex/mapr_test.h"

KeyReducer::KeyReducer(MapReduce *MR,
					EmitAcessorVecPtr reduce_vec)	
{
	EmitTypeAccessor a = *reduce_vec->begin();
	
	auto it =reduce_vec->begin();	
	
	it++;
	while (reduce_vec->size()!=1)
	{
		EmitTypeAccessor b = *it;
		MR->reduce(a.getEmit()->key, a.getEmit(), b.getEmit());
		reduce_vec->erase(it);		
	}
}

std::string ReduceDispatcher::getBatchFilenameById(int id)
{
	char filename[50];
	sprintf(filename, "batch%demits", id);
	return std::string(filename);
}

ReduceDispatcher::ReduceDispatcher(hThreadPool* pool, MapReduce *MR, EmitDumper *dumper):
		m_pool (pool),
		m_dumper (dumper),
		fd_deposit (boost::bind(&ReduceDispatcher::getBatchFilenameById,
					this, _1)),
		fd_rent (boost::bind(&ReduceDispatcher::getBatchFilenameById,
					this, _1)),
		reduce_tasks_counter(pool, 4, boost::bind(&ReduceDispatcher::onFinished, this))
{
	m_MR = MR->copy();
}

void ReduceDispatcher::addReduceResult(EmitType* emit, int emitter_id)
{
	//std::cout << "addReduceResult " << emit->key() << std::endl;
	hRWLockWrite rd_lock = hash_lock.write();
	
	int fd = fd_deposit.getAppendFile(emitter_id);
	EmitTypeAccessor emit_acc(emit, m_dumper, fd, emitter_id); // flushes to file
	
	
	
	std::unordered_map<int64_t, EmitAcessorVecPtr >::iterator it = 
			m_reduce_hash.find(emit->key);
	if (it!=m_reduce_hash.end())
	{
		//it->second.lock();
		it->second->push_back(emit_acc);
		//it->second.unlock();
	}
	else
	{
		EmitAcessorVecPtr q (new EmitAcessorVec);
		q->push_back(emit_acc);
		//rd_lock.unlock();
		//hRWLockWrite wr_lock = hash_lock.write();
		m_reduce_hash.insert(std::pair<int64_t, EmitAcessorVecPtr>(emit->key, q));
	}
	rd_lock.unlock();
}

bool ReduceDispatcher::reduceTask(EmitAcessorVecPtr emit_vec)
{
	KeyReducer reducer(m_MR, emit_vec);
	
	//m_nreduces_finished++;
	EmitType *emit = emit_vec->at(0).getEmit();
	dumpResultKey(emit->key, emit);
	emit_vec->clear();
	delete emit;
	return 0;
	/*
	if (m_nreduces_launched.load() == m_nreduces_finished.load())
	{
		if (m_all_reduces_launched.load())
		{
			if (finish_lock.trylock())
			{
				if (finished) return;
				finished = 1;
				std::cout << "FINISHED\n";
				//m_reduce_hash.clear();
				//sleep(10);
				//m_onAllReducesFinished (emit_queue_hash);
				finish_lock.unlock();
				exit(0);
			}
		}
	}*/
}

void ReduceDispatcher::onFinished()
{
	std::cout << "FINISHED\n";
//	_onFinished();
}

void ReduceDispatcher::restoreKey(EmitAcessorVecPtr emit_vec)
{
	for (int i = 0; i<emit_vec->size(); i++)
	{
		emit_vec->at(i).restore(m_dumper,
							fd_rent.getReadFile(emit_vec->at(i).getEmitterId()) );
	}
}

void ReduceDispatcher::dumpResultKey(int64_t key, EmitType* emit)
{
	std::string	dump = m_dumper->dump(emit);
	//std::cout << dump <<std::endl;
	size_t size = dump.size();
	
	iovec atom[3];
	
	atom[0].iov_base =  &key;
	atom[0].iov_len = sizeof(int64_t);
	
	atom[1].iov_base =  &size;
	atom[1].iov_len = sizeof(size_t);

	atom[2].iov_base = (void*)dump.data();
	atom[2].iov_len = size;

	writev(fd_result, atom, 2);
	dump.clear();
}

void ReduceDispatcher::start()
{
	std::cout << "ReduceDispatcher::start()" << std::endl;
	hRWLockWrite rd_lock = hash_lock.write();
	//if (finished) return;
	//fd_deposit.close();
	//m_all_reduces_launched = 0;
	finished = 0;

	fd_result = open("result", O_WRONLY | O_CREAT | O_APPEND,   
					   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	const int min_active_tasks = 8;
	const int max_keys_in_cache = 1000;

	std::cout << "Hash size: " << m_reduce_hash.size() << std::endl;


	// preload grouped emits, run reduce tasks
	int i = 0;
	auto hash_it = m_reduce_hash.begin();
	auto hash_end = m_reduce_hash.end();
	
	
	while (hash_it != hash_end)
	{
		restoreKey(hash_it->second);
		reduce_tasks_counter.addTask(new boost::function<bool()> (
					boost::bind(&ReduceDispatcher::reduceTask, this, hash_it->second)));
		hash_it++;
	}
	/*
	while (1)
	{	
		while (emit_vec_ram_cache.size()<max_keys_in_cache && hash_it != hash_end)
		{
			restoreKey(hash_it->second);
			emit_vec_ram_cache.push(hash_it->second);
			hash_it++;
		}
		
		//if (m_nreduces_launched.load()-m_nreduces_finished.load()< min_active_tasks)
		if (reduce_tasks_counter.countRunning() < min_active_tasks)
		{
			if (emit_vec_ram_cache.size()==0 && hash_it == hash_end)
			{
				break;
			}
			
			//m_nreduces_launched++;
			//reduce_tasks_counter.incLaunched();
			m_pool->addTask(new boost::function<void()> (
					boost::bind(&ReduceDispatcher::reduceTask, this, emit_vec_ram_cache.front())));
			emit_vec_ram_cache.pop();
		}
	}*/
	
	reduce_tasks_counter.setNoMoreTasks();
	/*
	sleep(1);
	hash_it = m_reduce_hash.begin();

	while (hash_it != hash_end)
	{
		std::cout << hash_it->first << " ";
		i = 0;
		while (i < hash_it->second->size())
		{
			InvertLine *line = (InvertLine*)hash_it->second->at(i).getEmit();
			std::cout << " |" << line->pages.size() << "| ";
			for (int j = 0; j<line->pages.size(); j++)
			std::cout << line->pages[j] << " ";
			i++;
		}
		std::cout << "\n";
		hash_it++;
	}
*/
}
