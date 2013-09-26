#ifndef MAPREDUCEDISPATCHER_H
#define  MAPREDUCEDISPATCHER_H

#include <unordered_map>
#include "../../threadpool/threadpool.h"
#include "mapreduce.h"
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
	    boost::function<void(std::shared_ptr<EmitHash>)> onBatchFinished);
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
    boost::function<void(EmitQueueHash*)> m_onAllReducesFinished;

    size_t m_nbatches;

    std::atomic<size_t> m_nbatches_finished;

    std::atomic<size_t> m_nreduces_launched;
    std::atomic<size_t> m_nreduces_finished;

    EmitQueueHash* emit_queue_hash;
    std::unordered_map<int64_t, std::shared_ptr<hLock> > m_key_locks;
    hRWLock queue_hash_lock;

    bool finished = 1;
    
    void lockKey(int64_t key);
    void unlockKey(int64_t key);

public:

    MRBatchDispatcher(MapReduce *MR, hThreadPool *pool, boost::function<void(EmitQueueHash*)> onAllBatchesFinished);

    void mapBatchTask(BatchAccessor* batch);   
    void reduceTask(int64_t key);

    void onBatchFinished(std::shared_ptr<EmitHash> emit_hash);

    void proceedBatches(std::shared_ptr< std::vector<BatchAccessor*> > batches);
    void mergeEmits(std::string filename);

    MRStats getStats();
};

#endif