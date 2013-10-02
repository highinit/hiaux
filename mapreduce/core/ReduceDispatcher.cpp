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

EmitTypeAccessor::EmitTypeAccessor(EmitType *result, std::string filepath)
{
	m_result = result;
	m_filepath = filepath;
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
	//std::cout << "addReduceResult " << emit->key() << std::endl;
	hRWLockWrite rd_lock = hash_lock.write();
	
	std::unordered_map<int64_t, EmitAcessorQueue >::iterator it = 
			m_reduce_hash.find(emit->key());
	if (it!=m_reduce_hash.end())
	{
		//it->second.lock();
		it->second.push(emit);
		//it->second.unlock();
	}
	else
	{
		EmitAcessorQueue q;
		q.push(emit);
		//rd_lock.unlock();
		//hRWLockWrite wr_lock = hash_lock.write();
		m_reduce_hash.insert(std::pair<int64_t, EmitAcessorQueue>(emit->key(), q));
	}
}

#include "../tests/mapr_test.h"

void ReduceDispatcher::start()
{
	auto hash_it = m_reduce_hash.begin();
	auto hash_end = m_reduce_hash.end();
	while (hash_it != hash_end)
	{
		std::cout << hash_it->first << "\n";
		
		while (hash_it->second.size()!=0)
		{
			InvertLine *line = (InvertLine*)hash_it->second.front()->m_result;
			std::cout << line->pages[0] << " ";
			hash_it->second.pop();
		}
		std::cout << "\n\n";
		hash_it++;
	}
}