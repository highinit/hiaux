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

#ifndef MAPREDUCEDISPATCHER_H
#define  MAPREDUCEDISPATCHER_H

#include <unordered_map>
#include "../../threadpool/threadpool.h"
#include "../../threadpool/tasklauncher.h"

#include "mapreduce.h"
#include "MRInterResult.h"
#include <queue>
#include <atomic>

class BatchMapper
{
    MapReduce *m_MR;
    std::shared_ptr<EmitHash> m_emit_hash;
    BatchAccessor* m_batch;
    MRStats m_stats;
public:
    
    BatchMapper(BatchAccessor* batch,
	    MapReduce *MR,
	    boost::function<void(std::shared_ptr<EmitHash>, int)> onBatchFinished,
		int batchid);
	~BatchMapper();
    void emit(int64_t key, EmitType* emit_value);
    MRStats getStats();
};

class MRBatchDispatcher
{
    MapReduce *m_MR;
	EmitDumper *m_emit_dumper;
	
    hThreadPool* m_pool;

    MRStats m_stats;
	TaskLauncher m_batch_tasks_launcher;
	TaskLauncher &m_flush_launcher;
	
	std::atomic<size_t> m_nbatches;
	
	std::string m_path;
	
    boost::function<void(MRInterResultPtr)> m_onGotResult;

public:

    MRBatchDispatcher(MapReduce *MR,
					EmitDumper *dumper,
					hThreadPool *pool,
					size_t nbatch_threads,
					TaskLauncher &flush_launcher,
					std::string path,
					boost::function<void(MRInterResultPtr)> onGotResult,
					boost::function<void()> onBatchingFinished);

    bool mapBatchTask(BatchAccessor* batch, int batchid);   

    void onBatchFinished(std::shared_ptr<EmitHash> emit_hash, int batchid);
    void addBatch(BatchAccessor* batch);
	void noMore();
	
    MRStats getStats();
};

#endif