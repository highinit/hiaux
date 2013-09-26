#include "ReduceDispatcher.h"

EmitTypeAccessor::EmitTypeAccessor(EmitType *m_result, std::string filepath)
{

}

EmitType *getResult()
{

}

void EmitTypeAccessor::dump()
{

}

void EmitTypeAccessor::restore()
{
	
}

KeyReducer::KeyReducer(std::string prefix,
					MapReduce *MR,
					EmitAcessorQueue reduce_queue):
		m_reduce_queue (reduce_queue),
		m_MR(MR)			
{

}

void ReduceDispatcher::addReduceResult(EmitTypeAccessorPtr emit)
{
	std::unordered_map<int64_t, EmitAcessorQueue >::iterator it = 
			m_reduce_hash.find(emit->key());
	if (it!=m_reduce_hash.end())
	{
		it->second->push(emit);
	}
	else
	{
		EmitAcessorQueue q(new std::queue<EmitTypeAccessorPtr>);
		q->push(emit);
	}
	
}
