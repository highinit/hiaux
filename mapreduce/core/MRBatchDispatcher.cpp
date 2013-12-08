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
#include <boost/atomic.hpp>

BatchMapper::BatchMapper(BatchAccessor* batch,
			MapReduce *MR,
			boost::function<void(boost::shared_ptr<EmitHash>,int)> onBatchFinished,
			int batchid)
{
	m_batch = batch;
	m_MR = MR->create();
	m_MR->setEmitF(boost::bind(&BatchMapper::emit, this, _1, _2));
	m_emit_hash.reset(new EmitHash);

	while (!m_batch->end())
	{
		m_stats.nmaps++;
		InputType *input = m_batch->getNextInput();
		if (input!=NULL)
		m_MR->map(input);
	}
	onBatchFinished(m_emit_hash, batchid);
}

BatchMapper::~BatchMapper()
{
	m_emit_hash->clear();
}

void BatchMapper::emit(uint64_t key, EmitType* emit_value)
{
	m_stats.nemits++;
	m_stats.nreduces++;
	std::tr1::unordered_map<uint64_t, EmitType* >::iterator it = m_emit_hash->find(key);
	if (it != m_emit_hash->end())
	{
		it->second = m_MR->reduce(key, it->second, emit_value);
	}
	else
	{
		m_emit_hash->insert(std::pair<uint64_t, EmitType* >(key, emit_value));
	}
}

MRStats BatchMapper::getStats()
{
    return m_stats;
}

bool MRBatchDispatcher::mapBatchTask(BatchAccessor* batch, int batchid)
{
	BatchMapper *mapper = new BatchMapper(batch, 
						m_MR, 
						boost::bind(&MRBatchDispatcher::onBatchFinished,
								this,
								_1, _2),
						batchid);
	m_stats += mapper->getStats();
	delete mapper;
	delete batch;
	return 0;
}

void MRBatchDispatcher::onBatchFinished(boost::shared_ptr<EmitHash> emit_hash, int batchid)
{
	EmitHash::iterator it = emit_hash->begin();
	EmitHash::iterator end = emit_hash->end();

	char filename[50];
	sprintf(filename, "batch%d", batchid);
	MRInterResultPtr inter(new MRInterResult(m_path+filename, m_MR, m_flush_launcher));

	while (it != end)
	{
		inter->addEmit(it->first, it->second);
		it++;
	}
	emit_hash->clear();
	m_onGotResult(inter);
}

MRBatchDispatcher::MRBatchDispatcher(MapReduce *MR,
					hThreadPool *pool,
					size_t nbatch_threads,
					TaskLauncher &flush_launcher,
					std::string path,
					boost::function<void(MRInterResultPtr)> onGotResult,
					boost::function<void()> onBatchingFinished):
		m_batch_tasks_launcher(pool,
							nbatch_threads,
							onBatchingFinished),
		m_onGotResult(onGotResult),
		m_MR(MR),
		m_pool(pool),
		m_flush_launcher(flush_launcher),
		m_nbatches(0),
		m_path(path),
		m_nomore(false)
{

}

void MRBatchDispatcher::addBatch(BatchAccessor* batch)
{
	int batchid = m_nbatches.fetch_add(1);
	m_batch_tasks_launcher.addTask(new boost::function<bool()>(
		boost::bind(&MRBatchDispatcher::mapBatchTask, this, batch, batchid)));
}

void MRBatchDispatcher::noMore()
{
	m_batch_tasks_launcher.setNoMoreTasks();
	m_nomore = true;
}

float MRBatchDispatcher::getFinishPercentage()
{
	if (m_nomore.load())
	{
		if (m_nbatches.load()!=0)
		    return 100.0f * m_batch_tasks_launcher.countFinished()/float(m_nbatches.load());
		else return 0.0f;
	}
	else
	{
		return 0.0f;
	}
}

MRStats MRBatchDispatcher::getStats()
{
    return m_stats;
}
