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
#include <unordered_map>
#include <queue>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

class InputType
{
public:
    virtual ~InputType() { }
};

class EmitType
{
public:
  int64_t key;
 
  virtual ~EmitType() { } 
};

class EmitDumper
{
public:
	virtual std::string dump(EmitType *emit) = 0;
	virtual EmitType* restore(std::string dumped) = 0;
};

typedef std::vector<EmitType*> EmitVec;
//typedef std::shared_ptr<EmitVec> EmitVecPtr;

typedef std::unordered_map<int64_t, EmitType*> EmitHash;
typedef std::queue<EmitType*> EmitQueue;

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
  //  std::string m_job_name;
 //   std::string m_node_name;
protected:
	boost::function<void(int64_t, EmitType*)> emit; 
    
public:
    
	MapReduce ();

	void setEmitF(boost::function<void(int64_t, EmitType*)> emitf);
	virtual void map(InputType* object) = 0;   
	virtual EmitType* reduce(int64_t emit_key, EmitType* a, EmitType* b) = 0;
	virtual void finilize(EmitType*) = 0;
	virtual MapReduce *copy() = 0;

	virtual ~MapReduce() { } 
};

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



#endif
