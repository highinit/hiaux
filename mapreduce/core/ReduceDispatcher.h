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
	size_t m_offset;
	int m_emitter_id;
public:

	EmitTypeAccessor(EmitType *emit, EmitDumper *dumper, int write_fd, int emitter_id);
	~EmitTypeAccessor() { }
	int getEmitterId();
	void restore(EmitDumper *dumper, int read_fd);
	EmitType *getEmit();
};

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
	hLock finish_lock;
	bool finished;
	
	EmitVecQueue emit_vec_ram_cache;
	
	AppendFileDeposit fd_deposit;
	ReadFileRent fd_rent;
	
	MapReduce *m_MR;
	EmitDumper *m_dumper;
	hThreadPool *m_pool;
	
	std::atomic<uint32_t> m_nreduces_launched;
	std::atomic<uint32_t> m_nreduces_finished;
	std::atomic<bool> m_all_reduces_launched;
	
	void restoreKey(EmitAcessorVecPtr emit_vec);
	int fd_result;
	void dumpResultKey(int64_t key, EmitType* emit);

public:
	
	ReduceDispatcher(hThreadPool* m_pool, MapReduce *MR, EmitDumper *dumper);
	
	std::string getBatchFilenameById(int id);
	
	void addReduceResult(EmitType* emit, int emitter_id);
	void start();
	void reduceTask(EmitAcessorVecPtr emit_vec);
};

#endif // REDUCE_DISPATCHER_H
