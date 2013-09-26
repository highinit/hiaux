#ifndef REDUCE_DISPATCHER_H
#define REDUCE_DISPATCHER_H

#include <unordered_map>

#include "../../hpoolserver/hpoolserver.h"
#include "mapreduce.h"
#include <queue>

class EmitTypeAccessor
{
	EmitType *m_result;
	std::string *m_filepath;
public:
	
	EmitTypeAccessor(EmitType *m_result, std::string filepath);
	EmitType *getResult();
	void dump();
	void restore();
	int64_t key()
	{
		return m_result->key;
	}
};

typedef boost::shared_ptr<EmitTypeAccessor> EmitTypeAccessorPtr;
typedef boost::shared_ptr< std::queue<EmitTypeAccessorPtr> > EmitAcessorQueue;

class KeyReducer
{
	MapReduce *m_MR;
	EmitAcessorQueue m_reduce_queue;
	
public:

	KeyReducer(std::string prefix,
				MapReduce *MR,
				EmitAcessorQueue reduce_queue);
	
};

class ReduceDispatcher
{
	std::unordered_map<int64_t, EmitAcessorQueue > m_reduce_hash;
public:
	
	void addReduceResult(EmitTypeAccessorPtr emit);
	void start();
};

#endif // REDUCE_DISPATCHER_H
