#ifndef MAPREDUCEDISPATCHER_H
#define  MAPREDUCEDISPATCHER_H

#include <boost/unordered_map.hpp>
#include "../../threadpool/threadpool.h"
#include "mapreduce.h"
#include <queue>

typedef std::tr1::unordered_map<int64_t, EmitType* > EmitHash;

//typedef boost::lockfree::queue<InputLocalChunkPtr> InputQueue;
//typedef boost::shared_ptr<MapReduce> MapReducePtr;
//typedef boost::shared_ptr<BatchAccessor> BatchAccessorPtr;
//typedef boost::shared_ptr<EmitHash> EmitHashPtr;

//typedef boost::lockfree::queue<EmitType*> EmitQueue;
//typedef std::queue<EmitType*> EmitQueue;
typedef std::vector<EmitType*> EmitQueue;
//typedef boost::shared_ptr< EmitQueue > EmitQueuePtr;

typedef std::tr1::unordered_map<int64_t, EmitQueue*> EmitQueueHash;
//typedef boost::shared_ptr< EmitQueueHash > EmitQueueHashPtr;

class BatchMapper
{
    MapReduce* m_MR;
    EmitHash* m_emit_hash;
    BatchAccessor* m_batch;
public:
    
    BatchMapper(BatchAccessor* batch, MapReduce* MR, boost::function<void(EmitHash*)> onBatchFinished)
    {
        m_batch = batch;
        m_MR = MR;
        m_MR->setEmitF(boost::bind(&BatchMapper::emit, this, _1, _2));
        m_emit_hash = new EmitHash;
        
        while (!m_batch->end())
        {
            m_MR->map(m_batch->getNextInput());
        }
        onBatchFinished(m_emit_hash);
    }
    
    void emit(int64_t key, EmitType* emit_value)
    {
       std::tr1::unordered_map<int64_t, EmitType* >::iterator it = m_emit_hash->find(key);
       if (it != m_emit_hash->end())
       {
           it->second = m_MR->reduce(key, it->second, emit_value);
       } else
       {
           m_emit_hash->insert(std::pair<int64_t, EmitType* >(key, emit_value));
       }
    }
    
};

class MapReduceDispatcher
{
    MapReduce* m_MR;
    hThreadPool* m_pool;
   
    // !!! use rw locks
    
    
    
public:
    
    EmitQueueHash* emit_queue_hash;
    
    void onBatchFinished(EmitHash* emit_hash)
    {
        // push to emit_queue_hash queues
        EmitHash::iterator it = emit_hash->begin();
        EmitHash::iterator end = emit_hash->end();
        
        while (it != end)
        {
            EmitQueueHash::iterator queue_hash_it = emit_queue_hash->find(it->first);
            if (queue_hash_it != emit_queue_hash->end())
            {
                queue_hash_it->second->push_back(it->second);
            } else
            {
                EmitQueue* queue_ptr = new EmitQueue;
                queue_ptr->push_back(it->second);
                emit_queue_hash->insert(std::pair<int64_t, EmitQueue*> (it->first, queue_ptr)); 
            }
            it++;
        }
        
        // decide to map next batch or reduce node level
    }
    
    MapReduceDispatcher(MapReduce* MR)
    {
        m_MR = MR;
        emit_queue_hash = new EmitQueueHash;
        //m_pool = pool;
    }
    
    BatchMapper* proceed(BatchAccessor* batch)
    {
        return new BatchMapper(batch, m_MR, boost::bind(&MapReduceDispatcher::onBatchFinished, this, _1));
    }
    
};

#endif