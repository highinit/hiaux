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

#ifndef MAPREDUCE_H
#define  MAPREDUCE_H

#include <vector>
#include <tr1/unordered_map>
#include <queue>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/atomic.hpp>

class MRStats
{
public:
	boost::atomic<size_t> nmaps;
	boost::atomic<size_t> nemits;
	boost::atomic<size_t> nreduces;
	MRStats();
	MRStats(MRStats &a);
	MRStats& operator+=(const MRStats &a);
	MRStats& operator=(const MRStats &a);
};


class InputType
{
public:
	virtual ~InputType() { }
};


class EmitType
{
public: 
	virtual ~EmitType() { } 
};

typedef std::tr1::unordered_map<uint64_t, EmitType*> EmitHash;

class BatchAccessor
{
public:
	BatchAccessor() { }
	virtual bool end() = 0;
	virtual InputType *getNextInput() = 0;
	virtual ~BatchAccessor() { }
};

class MapReduce
{
protected:
	boost::function<void(uint64_t, EmitType*)> emit; 

public:
	MapReduce ();
	void setEmitF(boost::function<void(uint64_t, EmitType*)> emitf);

	virtual void map(InputType* object) = 0;
	virtual EmitType* reduce(uint64_t key, EmitType* a, EmitType* b) = 0;
	virtual void finilize(EmitType*) = 0;

	virtual std::string dumpEmit(EmitType *emit) = 0;
	virtual EmitType* restoreEmit(std::string dump) = 0;

	virtual MapReduce *create() = 0;
	virtual ~MapReduce() { }
};


#endif
