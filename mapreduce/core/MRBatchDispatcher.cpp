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

BatchMapper::~BatchMapper()
{
	m_emit_hash->clear();
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

void MRBatchDispatcher::mapBatchTask(BatchAccessor* batch, int batchid)
{
	BatchMapper *mapper = new BatchMapper(batch, 
									m_MR, 
									boost::bind(&MRBatchDispatcher::onBatchFinished,
												this,
												_1, _2), batchid);
	m_stats += mapper->getStats();
	delete mapper;
	delete batch;
}

void MRBatchDispatcher::onBatchFinished(std::shared_ptr<EmitHash> emit_hash, int batchid)
{
	EmitHash::iterator it = emit_hash->begin();
	EmitHash::iterator end = emit_hash->end();
	
	std::cout << "batch finished. flushing \n";
	while (it != end)
	{
		reducer->addReduceResult(it->second, batchid); 
		//delete it->second;
		it++;
	}
	emit_hash->clear();
	std::cout << "flushing finished\n";
	// atom
	m_nbatches_finished++;
	
	if (m_nbatches_finished.load() == m_nbatches)
	{
		if (finish_lock.trylock())
		{
			if (finished) return;
			finished = 1;
			std::cout << "Batching finished\n";
			std::cout << "maps: " << m_stats.nmaps << std::endl; 
			std::cout << "emits: " << m_stats.nemits << std::endl;
			reducer->start();
			finish_lock.unlock();
		}
	}
}

MRBatchDispatcher::MRBatchDispatcher(MapReduce* MR,
								EmitDumper *dumper,
                               hThreadPool *pool, 
                               boost::function<void()> onAllReducesFinished)
{
	m_nbatches = 0;
	m_nbatches_launched = 0;
	m_nbatches_finished = 0;

	finished = 0;

	m_MR = MR;
	m_emit_dumper = dumper;
//	emit_queue_hash = new EmitQueueHash;
	m_pool = pool;
	m_onAllReducesFinished = onAllReducesFinished;
	m_nreduces_launched =  0;
	m_nreduces_finished = 0;
	reducer = new ReduceDispatcher(pool, MR, dumper);
}

void MRBatchDispatcher::proceedBatches(
	std::shared_ptr< std::vector<BatchAccessor*> > batches)
{
	const int max_running_batchings = 1;
	
	m_nbatches = batches->size();
	for (int i = 0; i<batches->size(); i++)
	{
		while (m_nbatches_launched.load()-m_nbatches_finished.load()>=max_running_batchings)
		{
			sleep(1);
		}
		//std::cout << "launch batch \n";
		m_nbatches_launched++;
		m_pool->addTask(new boost::function<void()>(
			boost::bind(&MRBatchDispatcher::mapBatchTask, this, batches->at(i), i)));
	}
	batches->clear();
}

MRStats MRBatchDispatcher::getStats()
{
    return m_stats;
}
