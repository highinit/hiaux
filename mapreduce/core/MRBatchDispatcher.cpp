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

#include "MRBatchDispatcher.h"
#include <atomic>

MRStats::MRStats()
{
	nmaps = 0;
	nemits = 0;
	nreduces = 0;
}

MRStats& MRStats::operator+=(const MRStats &a)
{
	lock.lock();
	nmaps += a.nmaps;
	nemits += a.nemits;
	nreduces += a.nreduces;
	lock.unlock();
	return *this;
}

MRStats& MRStats::operator=(const MRStats &a)
{
	nmaps = a.nmaps;
	nemits = a.nemits;
	nreduces = a.nreduces;
	return *this;
}

BatchMapper::BatchMapper(BatchAccessor* batch, 
                        MapReduce *MR,
                        boost::function<void(std::shared_ptr<EmitHash>,int)> onBatchFinished,
						int batchid)
{
	m_batch = batch;
	m_MR = MR->copy();
	m_MR->setEmitF(boost::bind(&BatchMapper::emit, this, _1, _2));
	m_emit_hash.reset(new EmitHash);

	while (!m_batch->end())
	{
		m_stats.nmaps++;
		m_MR->map(m_batch->getNextInput());
	}
	onBatchFinished(m_emit_hash, batchid);
}

void BatchMapper::emit(int64_t key, EmitType* emit_value)
{
	m_stats.nemits++;
	std::unordered_map<int64_t, EmitType* >::iterator it = m_emit_hash->find(key);
	if (it != m_emit_hash->end())
	{
		m_stats.nreduces++;
		it->second = m_MR->reduce(key, it->second, emit_value);
	}
	else
	{
		m_emit_hash->insert(std::pair<int64_t, EmitType* >(key, emit_value));
	}
}

MRStats BatchMapper::getStats()
{
    return m_stats;
}

NodeReducer::NodeReducer(int64_t key, 
					std::shared_ptr<EmitQueue> emit_queue, 
					MapReduce* MR)
{
    while (emit_queue->size()!=1)
    {
        m_stats.nreduces++;
        EmitType *a = emit_queue->front();
        emit_queue->pop();
        EmitType *b = emit_queue->front();
        emit_queue->pop();
        emit_queue->push(MR->reduce(key, a, b)); 
    }
}

MRStats NodeReducer::getStats()
{
    return m_stats;
}
/*
void MRBatchDispatcher::lockKey(int64_t key)
{
	hRWLockWrite hash_write_lock = queue_hash_lock.write();
	
	std::unordered_map<int64_t, std::shared_ptr<hLock> >:: iterator it = 
			m_key_locks.find(key);
	
	std::shared_ptr<hLock> lock;
	
	if (it!=m_key_locks.end())
	{
		lock = it->second;
	}
	else
	{
		lock.reset(new hLock);
		m_key_locks.insert( std::pair<int64_t, std::shared_ptr<hLock> >(key, lock));
	}
	
	hash_write_lock.unlock();
	
	lock->lock();
	
}

void MRBatchDispatcher::unlockKey(int64_t key)
{
	hRWLockWrite hash_write_lock = queue_hash_lock.write();
	
	std::unordered_map<int64_t, std::shared_ptr<hLock> >:: iterator it = 
			m_key_locks.find(key);
	
	std::shared_ptr<hLock> lock;
	
	if (it!=m_key_locks.end())
	{
		it->second->unlock();
	}
	
	//hash_write_lock.unlock();
}
*/
void MRBatchDispatcher::mapBatchTask(BatchAccessor* batch, int batchid)
{
	BatchMapper *mapper = new BatchMapper(batch, 
									m_MR, 
									boost::bind(&MRBatchDispatcher::onBatchFinished,
												this,
												_1, _2), batchid);
	m_stats += mapper->getStats();
	delete mapper;
}

/*
void MRBatchDispatcher::reduceTask(int64_t key)
{
	//std::cout << "reduceTask lockKey\n";
	
	lockKey(key);
	//std::cout << "reduceTask queue_hash_lock.lock\n";
	hRWLockWrite writelock = queue_hash_lock.write();
	EmitQueueHash::iterator it = emit_queue_hash->find(key);
	
	
	if (it != emit_queue_hash->end())
	{
			std::shared_ptr<EmitQueue> emit_queue = it->second;
			writelock.unlock();
			NodeReducer reducer(key, emit_queue, m_MR);
			m_stats += reducer.getStats();
	}
	else
	{
		writelock.unlock();
	}
	
	unlockKey(key);
	m_nreduces_finished++;

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
	}
}*/

void MRBatchDispatcher::onBatchFinished(std::shared_ptr<EmitHash> emit_hash, int batchid)
{
	//hRWLockWrite writelock = queue_hash_lock.write();
	EmitHash::iterator it = emit_hash->begin();
	EmitHash::iterator end = emit_hash->end();
	
	while (it != end)
	{
		reducer->addReduceResult(it->second, batchid); 
		//EmitTypeAccessorPtr( 
	//	EmitTypeAccessorPtr(new EmitTypeAccessor(it->second, "FIXTHISSHIT") ) ) );
		it++;
	}
	// atom
	m_nbatches_finished++;
	
	//std::cout << "bf: " << m_nbatches_finished.load() << std::endl;
	
	if (m_nbatches_finished.load() == m_nbatches)
	{
		if (finish_lock.trylock())
		{
			if (finished) return;
			finished = 1;
			std::cout << "Batching finished\n";
			//sleep(10);
			reducer->start();
			//m_onAllReducesFinished ();
			finish_lock.unlock();
		}
	}
}

MRBatchDispatcher::MRBatchDispatcher(MapReduce* MR, 
                               hThreadPool *pool, 
                               boost::function<void()> onAllReducesFinished)
{
	m_nbatches = 0;
	m_nbatches_finished = 0;

	finished = 0;

	m_MR = MR;
//	emit_queue_hash = new EmitQueueHash;
	m_pool = pool;
	m_onAllReducesFinished = onAllReducesFinished;
	m_nreduces_launched =  0;
	m_nreduces_finished = 0;
	reducer.reset(new ReduceDispatcher(MR));
}

void MRBatchDispatcher::proceedBatches(
	std::shared_ptr< std::vector<BatchAccessor*> > batches)
{
	m_nbatches = batches->size();
	for (int i = 0; i<batches->size(); i++)
	{
		m_pool->addTask(new boost::function<void()>(
			boost::bind(&MRBatchDispatcher::mapBatchTask, this, batches->at(i), i)));
	}
}

MRStats MRBatchDispatcher::getStats()
{
    return m_stats;
}
