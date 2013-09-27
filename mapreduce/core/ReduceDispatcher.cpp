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
