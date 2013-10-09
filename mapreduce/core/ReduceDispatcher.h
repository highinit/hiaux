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

#include "FileCartulary.h"

class EmitTypeAccessor
{
	EmitType *m_emit;
	size_t offset;
	size_t size;
public:

	EmitTypeAccessor(EmitType *emit, int write_fd);
	~EmitTypeAccessor() { }
	void restore(int read_fd);
	EmitType *getEmit();
};

//typedef boost::shared_ptr<EmitTypeAccessor> EmitTypeAccessorPtr;

//public std::queue<EmitType*>
class EmitAcessorVec : public std::vector<EmitTypeAccessor> 
{
	EmitAcessorVec (EmitAcessorVec & a);
	EmitAcessorVec& operator=(EmitAcessorVec&);
	hLock m_lock;
	
public:
	
	EmitAcessorVec()
	{
	}
	
	void lock()
	{
		m_lock.lock();
	}
	
	void unlock()
	{
		m_lock.unlock();
	}
};

typedef boost::shared_ptr<EmitAcessorVec> EmitAcessorVecPtr;

class KeyReducer
{	
public:

	KeyReducer(MapReduce *MR,
				EmitAcessorVecPtr reduce_vec);
	
};

class EmitVecQueue : public std::queue<EmitAcessorVecPtr>
{
	EmitVecQueue (EmitVecQueue & a);
	EmitVecQueue& operator=(EmitVecQueue&);
	hLock m_lock;
public:
	
	EmitVecQueue()
	{
	}
	
	void lock()
	{
		m_lock.lock();
	}
	
	void unlock()
	{
		m_lock.unlock();
	}
	
};

class ReduceDispatcher
{
	std::unordered_map<int64_t, EmitAcessorVecPtr > m_reduce_hash;
	hRWLock hash_lock;
	
	EmitVecQueue emit_vec_ram_cache;
	
	AppendFileDeposit fd_deposit;
	MapReduce *m_MR;
	
	hThreadPool* m_pool;
	
public:
	
	ReduceDispatcher(hThreadPool* m_pool, MapReduce *MR);
	
	std::string getBatchFilenameById(int id);
	
	void addReduceResult(EmitType* emit, int dumpfiled);
	void start();
	void reduceTask(EmitAcessorVecPtr emit_vec);
};

#endif // REDUCE_DISPATCHER_H
