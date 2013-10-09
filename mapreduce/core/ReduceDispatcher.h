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
	EmitType *m_emit;
	int fd;
	size_t offset;
	size_t size;
public:

	EmitTypeAccessor(EmitType *emit, int write_fd);
	void restore(int read_fd);
	EmitType *getEmit();
};

//typedef boost::shared_ptr<EmitTypeAccessor> EmitTypeAccessorPtr;

//public std::queue<EmitType*>
class EmitAcessorQueue : std::queue<EmitTypeAccessor> 
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
	// writer id, fd
	std::unordered_map<int, int> write_fd;
	
	hRWLock hash_lock;
	MapReduce *m_MR;
	
	std::string getFilenameById(int id);
	int readFd(int);
public:
	
	ReduceDispatcher(MapReduce *MR);
	
	void addReduceResult(EmitType* emit, int dumpfiled);
	void start();
};

#endif // REDUCE_DISPATCHER_H
