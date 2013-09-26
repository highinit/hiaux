#ifndef MAPREDUCE_H
#define  MAPREDUCE_H

#include <vector>
#include <unordered_map>
#include <queue>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

class InputType
{
public:
    
};

class EmitType
{
public:
  int64_t key;
  virtual virtual std::string dump() = 0;
  virtual void restore(std::string dumped) = 0;
};

typedef std::unordered_map<int64_t, EmitType* > EmitHash;
typedef std::queue<EmitType*> EmitQueue;
typedef std::unordered_map<int64_t, std::shared_ptr<EmitQueue> > EmitQueueHash;

class BatchAccessor
{
public:
    virtual bool end() = 0;
    virtual InputType *getNextInput() = 0;
};

class MapReduce
{   
    std::string m_job_name;
    std::string m_node_name;
protected:
    boost::function<void(int64_t, EmitType*)> emit; 
    
public:
    
    MapReduce (std::string job_name, std::string node_name);
    
    void setEmitF(boost::function<void(int64_t, EmitType*)> emitf);
    virtual void map(InputType* object) = 0;   
    virtual EmitType* reduce(int64_t emit_key, EmitType* a, EmitType* b) = 0;
    virtual void finilize(EmitType*) = 0;
    virtual MapReduce *copy() = 0;
};

#endif
