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

#include "../tests/mapr_test.h"

EmitTypeAccessor::EmitTypeAccessor(EmitType *emit, int append_fd):
	m_emit(emit)		
{
	// dump and remember offset and size
}

void EmitTypeAccessor::restore(int read_fd)
{
	
}

EmitType *EmitTypeAccessor::getEmit()
{
	return m_emit;
}

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

ReduceDispatcher::ReduceDispatcher(hThreadPool* pool, MapReduce *MR):
		m_pool (pool),
		
		fd_deposit (boost::bind(&ReduceDispatcher::getBatchFilenameById,
					this, _1))
{
	m_MR = MR->copy();
}

void ReduceDispatcher::addReduceResult(EmitType* emit, int batchid)
{
	//std::cout << "addReduceResult " << emit->key() << std::endl;
	int fd = fd_deposit.getAppendFile(batchid);
	EmitTypeAccessor emit_acc(emit, fd); // flushes to batch file
	
	hRWLockWrite rd_lock = hash_lock.write();
	
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

void ReduceDispatcher::reduceTask(EmitAcessorVecPtr emit_vec)
{
	KeyReducer reducer(m_MR, emit_vec);
	
	/*m_nreduces_finished++;

	if (m_nreduces_launched.load() == m_nreduces_finished.load())
	{
		if (m_nbatches_finished.load() == m_nbatches)
		{
			if (finish_lock.trylock())
			{
				if (finished) return;
				finished = 1;
				std::cout << "FINISHED\n";
				//sleep(10);
				m_onAllReducesFinished (emit_queue_hash);
				finish_lock.unlock();
			}
		}
	}*/
}

void ReduceDispatcher::start()
{	
	hRWLockWrite rd_lock = hash_lock.write();
	
	std::cout << "Hash size: " << m_reduce_hash.size() << std::endl;

	
	// preload grouped emits, run reduce tasks
	int i = 0;
	auto hash_it = m_reduce_hash.begin();
	auto hash_end = m_reduce_hash.end();
	
	
	while (hash_it != hash_end)
	{
		//emit_vec_ram_cache.lock();
		//emit_vec_ram_cache.push(hash_it->second);
		//emit_vec_ram_cache.unlock();
		m_pool->addTask(new boost::function<void()> (
				boost::bind(&ReduceDispatcher::reduceTask, this, hash_it->second)));
		//sleep(1);
		//if (i==2)
		//break;
		i++;
		hash_it++;
	}
	
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
	
}
