#ifndef MAPREDUCEDISPATCHER_H
#define  MAPREDUCEDISPATCHER_H


#include <boost/unordered_map.hpp>
#include "../../threadpool/threadpool.h"
#include "mapreduce.h"


typedef boost::lockfree::queue<InputLocalChunkPtr> InputQueue;
typedef boost::shared_ptr<MapReduce> MapReducePtr;


class MapperChunkHandler
{
    MapReducePtr MR;
    InputLocalChunkPtr chunk;
    EmitHash emit_hash;
public:
    
    void HandleChunk()
    {
        
    }
    
    void emit(int64_t key, EmitTypePtr emit_value)
    {
        
    }
    
};

class MapReduceDispatcher
{
    boost::shared_ptr<MapReduce> MR;
    boost::shared_ptr<hThreadPool> pool;
    boost::shared_ptr<InputQueue> Input;
   
public:
    
    
    
};

#endif