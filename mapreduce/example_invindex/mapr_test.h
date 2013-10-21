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
	std::vector<uint64_t> words;
	
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

class InvertLine : public EmitType
{
public:
	std::vector<uint64_t> pages;

	InvertLine()
	{
		
	}
	
	~InvertLine()
	{
		pages.clear();
	}
	
	void print()
	{
		std::cout << "(" << pages.size() << ") | ";
		for (int i = 0; i<pages.size(); i++)
		{
			std::cout << pages[i] << " ";
		}
		std::cout << std::endl;
	}

};

class MapReduceInvertIndex : public MapReduce
{
public:

	MapReduceInvertIndex();

	virtual void map(InputType* object);    
	virtual EmitType* reduce(uint64_t emit_key, EmitType* _a, EmitType* _b);
	virtual void finilize(EmitType* result);
	virtual MapReduce *copy();
	
	~MapReduceInvertIndex() { } 
};

class InvertLineDumper : public EmitDumper
{
public:
	virtual std::string dump(EmitType *emit);
	virtual EmitType* restore(std::string dumped);
};

void onAllBatchesFinished();


#endif	/* MAPR_TEST_H */
