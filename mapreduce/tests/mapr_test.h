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
	int m_b;
	int m_e;
public:
	int64_t id;
	std::vector<int64_t> words;
	
	void fill();
	Document(int b, int e, int id);
	~Document();
};

class DocumentBatch : public BatchAccessor
{
	bool isend;
	std::queue < Document*> m_docs;
public:

	DocumentBatch(std::vector < Document* > &docs);

	virtual bool end();
	virtual InputType* getNextInput();
	~DocumentBatch();
};

class MapReduceInvertIndex : public MapReduce
{
public:

	MapReduceInvertIndex();

	virtual void map(InputType* object);    
	virtual EmitType* reduce(int64_t emit_key, EmitType* _a, EmitType* _b);
	virtual void finilize(EmitType* result);
	virtual MapReduce *copy();
	
	~MapReduceInvertIndex() { } 
};

class InvertLine : public EmitType
{
public:
	std::vector<int64_t> pages;

	InvertLine(int64_t _key)
	{
		key = _key;
	}
	
	~InvertLine()
	{
		pages.clear();
	}
};

class InvertLineDumper : public EmitDumper
{
public:
	virtual std::string dump(EmitType *emit);
	virtual EmitType* restore(std::string dumped);
};

void onAllBatchesFinished();


#endif	/* MAPR_TEST_H */
