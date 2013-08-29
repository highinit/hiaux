#ifndef MAPREDUCE_H
#define  MAPREDUCE_H

#include <vector>
#include <boost/shared_ptr.hpp>

class InputType
{
public:
    
};

class EmitType
{
public:
    
};

class ReduceResult
{
public:
    int64_t key;
    EmitType value;
};

typedef std::vector<InputType> InputLocalChunk;
typedef boost::shared_ptr<InputLocalChunk> InputLocalChunkPtr;
typedef boost::shared_ptr<EmitType> EmitTypePtr;
typedef boost::unordered_map<int64_t, std::vector<EmitTypePtr> > EmitHash;
typedef boost::shared_ptr<EmitHash> EmitHashPtr;

class MapReduce
{
    void emit(int64_t key, EmitType &e);
    std::string job_name;
    std::string node_name;
public:
    
    virtual void map(int64_t map_key, InputType &object) = 0;   
    virtual ReduceResult reduce(int64_t emit_key, std::vector<EmitType> &emits) = 0;
    
    virtual InputType getInput(int64_t id) = 0;
    virtual void saveReduceResult(ReduceResult result, FILE *f) = 0;
    virtual ReduceResult getNextLocalReduceResult(FILE *f) = 0;
    virtual void uploadResult(ReduceResult result);
};

#endif
