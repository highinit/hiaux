/* 
 * File:   mapr_test.h
 * Author: phrk
 *
 * Created on September 14, 2013, 2:31 PM
 */

#ifndef MAPR_TEST_H
#define	MAPR_TEST_H

class Document
{
public:
	int64_t id;
	std::vector<int64_t> words;

	Document(int b, int e, int id);
};

class DocumentBatch : public BatchAccessor
{
	bool isend;
	std::queue < Document*> m_docs;
public:

	DocumentBatch(std::vector < Document* > &docs);

	virtual bool end();
	virtual InputType* getNextInput();
};

class MapReduceInvertIndex : public MapReduce
{
public:

	MapReduceInvertIndex(std::string job_name, std::string node_name);

	virtual void map(InputType* object);    
	virtual EmitType* reduce(int64_t emit_key, EmitType* _a, EmitType* _b);
	virtual void finilize(EmitType* result);
	virtual MapReduce *copy();
};

class InvertLine : public EmitType
{
public:
	std::vector<int64_t> pages;

	virtual std::string dump();
	virtual void restore(std::string dumped);

	InvertLine(int64_t _key)
	{
		key = _key;
	}
	
	~InvertLine()
	{
		pages.clear();
	}
};

void onAllBatchesFinished();


#endif	/* MAPR_TEST_H */
