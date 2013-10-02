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

#ifndef REDUCE_DISPATCHER_H
#define REDUCE_DISPATCHER_H

#include <unordered_map>

#include "../../hpoolserver/hpoolserver.h"
#include "mapreduce.h"
#include <queue>

class EmitTypeAccessor
{
	
	std::string m_filepath;
public:
	EmitType *m_result;
	
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

class EmitAcessorQueue : public std::queue<EmitTypeAccessorPtr > 
{
public:
	hLock m_lock;
	
	
	void lock()
	{
		m_lock.lock();
	}
	
	void unlock()
	{
		m_lock.unlock();
	}
};

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
	hRWLock hash_lock;
public:
	
	void addReduceResult(EmitTypeAccessorPtr emit);
	void start();
};

#endif // REDUCE_DISPATCHER_H
