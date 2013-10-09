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
#include "mapreduce.h"
#include "ReduceDispatcher.h"
#include <queue>
#include <atomic>

class MRStats
{
    hLock lock;
public:
    size_t nmaps;
    size_t nemits;
    size_t nreduces;
    MRStats();
    MRStats& operator+=(const MRStats &a);
    MRStats& operator=(const MRStats &a);
};

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
    void emit(int64_t key, EmitType* emit_value);
    MRStats getStats();
};

class NodeReducer
{
    MRStats m_stats;
public:
    
    NodeReducer(int64_t key, std::shared_ptr<EmitQueue> emit_queue, MapReduce* MR);
    MRStats getStats();    
};

class MRBatchDispatcher
{
    MapReduce *m_MR;
    hThreadPool* m_pool;

    MRStats m_stats;

    hLock finish_lock;
    boost::function<void()> m_onAllReducesFinished;

    size_t m_nbatches;

    std::atomic<size_t> m_nbatches_finished;

    std::atomic<size_t> m_nreduces_launched;
    std::atomic<size_t> m_nreduces_finished;

    bool finished = 1;
    
	std::shared_ptr<ReduceDispatcher> reducer; 
	
    void lockKey(int64_t key);
    void unlockKey(int64_t key);

public:

    MRBatchDispatcher(MapReduce *MR, hThreadPool *pool, boost::function<void()> onAllBatchesFinished);

    void mapBatchTask(BatchAccessor* batch, int batchid);   
    //void reduceTask(int64_t key);

    void onBatchFinished(std::shared_ptr<EmitHash> emit_hash, int batchid);

    void proceedBatches(std::shared_ptr< std::vector<BatchAccessor*> > batches);
    //void mergeEmits(std::string filename);

    MRStats getStats();
};

#endif