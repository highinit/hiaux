/* 
 * File:   mapr_test.cpp
 * Author: phrk
 *
 * Created on August 24, 2013, 1:58 AM
 */
#include <vector>
#include <cstdlib>
#include "../core/mapreduce.h"
#include "../core/MRBatchDispatcher.h"

#include "../core/ReduceDispatcher.h"

#include "mapr_test.h"
#include <iostream>

using namespace std;

std::string InvertLine::dump()
{

}

void InvertLine::restore(std::string dumped)
{

}

MapReduceInvertIndex::MapReduceInvertIndex(std::string job_name, std::string node_name) :
        MapReduce(job_name, node_name)
{

} 

void MapReduceInvertIndex::map(InputType* object)
{
	//std::cout << "MapReduceInvertIndex::map\n";
	
	Document *in_obj = (Document *) object ;
	//std::cout << in_obj->id << std::endl;
	for (int i = 0; i<in_obj->words.size(); i++)
	{
		InvertLine *line = new InvertLine(in_obj->words[i]);
		line->pages.push_back(in_obj->id);
		emit(in_obj->words[i], line);
	}
}

EmitType* MapReduceInvertIndex::reduce(int64_t emit_key, EmitType* _a, EmitType* _b)
{
//	std::cout << "reduce\n";
	InvertLine *a = (InvertLine*) _a;
	InvertLine *b = (InvertLine*) _b;

	for (int i = 0; i<b->pages.size(); i++)
	{
		a->pages.push_back(b->pages[i]);
	}

	delete b;
	return a;
}

void MapReduceInvertIndex::finilize(EmitType* result) 
{
    
}

MapReduce *MapReduceInvertIndex::copy()
{
	return new MapReduceInvertIndex(*this);
}

DocumentBatch::DocumentBatch(std::vector < Document*> &docs)
{
	isend = 0;
	for (int i = 0 ; i<docs.size(); i++)
	m_docs.push(docs[i]);
}

Document::Document(int b, int e, int docid)
{
	id = docid;
	for (int i =b ; i<e; i++)
	words.push_back(i);        
}

bool DocumentBatch::end()
{
	return isend;
}

InputType* DocumentBatch::getNextInput()
{
	
	Document *doc_doc = m_docs.front();
	InputType* doc = (InputType*) doc_doc;
	
	m_docs.pop();
	//std::cout << "id:"<< doc_doc->id << "\n";
	if (m_docs.size()==0) isend = 1;
	return  doc;
}

MRBatchDispatcher *mr_disp;
int64_t start_time;

//#define PRINT

void onAllReducesFinished()
{
/*
	std::cout << "onAllReducesFinished" << std::endl;
	EmitQueueHash::iterator it = hash->begin();
	while (it != hash->end())
	{
#ifdef PRINT		
	  std::cout << "key:" << it->first;
#endif        
		while (it->second->size() != 0)
		{
#ifdef PRINT	
		   std:cout << ":(";
#endif
		   InvertLine* line = (InvertLine*) it->second->front();
			it->second->pop();
#ifdef PRINT	
			std::cout << line->pages.size() << ") "; 
			for (int i =0 ; i<line->pages.size(); i++)
			std::cout << " " << line->pages[i] << ", ";
#endif
			delete line;
		}
#ifdef PRINT	
	  std::cout << std::endl;
#endif
		it++;
	}
	delete hash;
	MRStats stats = mr_disp->getStats();
	std::cout << "maps: " << stats.nmaps << std::endl;
	std::cout << "emits: " << stats.nemits << std::endl;
	std::cout << "reduces: " << stats.nreduces << std::endl;
	std::cout << "time took: " << time(0) - start_time << std::endl;*/
}

int main(int argc, char** argv) 
{
	MapReduceInvertIndex *MR = new MapReduceInvertIndex("InvertIndex", "localhost");
	hThreadPool *pool = new hThreadPool(5);
	mr_disp = new MRBatchDispatcher(MR, pool, boost::bind(&onAllReducesFinished));
/*	DocumentBatch *batch = new DocumentBatch(0, 10, 42);
	DocumentBatch *batch2 = new DocumentBatch(5, 20, 13);
	DocumentBatch *batch3 = new DocumentBatch(2, 7, 6);
	DocumentBatch *batch4 = new DocumentBatch(0, 20, 2);
	DocumentBatch *batch5 = new DocumentBatch(0, 20, 3);
*/	
	std::shared_ptr< std::vector<BatchAccessor*> > 
				batches (new std::vector<BatchAccessor*>);
/*	batches->push_back(batch);
	batches->push_back(batch2);
	batches->push_back(batch3);
	batches->push_back(batch4);
	batches->push_back(batch5);
*/
	
	std::vector<Document*> docs;
	
	const int input_size = 10;
	
	for (int i = 0; i<=input_size; i++)
	{
		Document *doc = new Document(i, i+20, i);
		docs.push_back( doc );
		if (i%(input_size/4) ==0)
		{
			DocumentBatch *batch = new DocumentBatch(docs);
			docs.clear();
			batches->push_back(batch);
		}
	}
	
	start_time = time(0);
	mr_disp->proceedBatches(batches);
	pool->run();
	pool->join();

	return 0;
}

