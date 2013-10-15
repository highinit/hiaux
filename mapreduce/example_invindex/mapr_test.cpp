/* 
 * File:   mapr_test.cpp
 * Author: phrk
 *
 * Created on August 24, 2013, 1:58 AM
 */
#include <vector>
//#include <cstdlib>
#include "../core/mapreduce.h"
#include "../core/MRBatchDispatcher.h"

#include "../core/ReduceDispatcher.h"

#include "mapr_test.h"
#include <iostream>

#include "invertline.pb.h"

using namespace std;

std::string InvertLineDumper::dump(EmitType *emit)
{
	InvertLine *line = (InvertLine*) emit;
	//std::cout << "dump line->pages.size(): " << line->pages.size() << std::endl;
	mapr_test::InvertLine pb_line;
	
	pb_line.set_key(line->key);
	for (int i = 0; i<line->pages.size(); i++)
		pb_line.add_pages(line->pages[i]);

	std::string ret =  pb_line.SerializeAsString();
	pb_line.Clear();
	return ret;
}

EmitType* InvertLineDumper::restore(std::string dumped)
{
	InvertLine *line = new InvertLine(0);
	mapr_test::InvertLine pb_line;
	pb_line.ParseFromString(dumped);
	
	line->key = pb_line.key();
	for (int i = 0; i<pb_line.pages_size(); i++)
	{
		line->pages.push_back(pb_line.pages(i));
	}
	pb_line.Clear();
//	std::cout << "restore line->pages.size(): " << line->pages.size() 
//			<< " " << line->pages[0]
//			<< std::endl;
	return (EmitType*) line;
}

MapReduceInvertIndex::MapReduceInvertIndex() 
   //     MapReduce(job_name, node_name)
{

}

void MapReduceInvertIndex::map(InputType* object)
{	
	Document *in_obj = (Document *) object ;

	for (int i = 0; i<in_obj->words.size(); i++)
	{
		InvertLine *line = new InvertLine(in_obj->words[i]);
		line->pages.push_back(in_obj->id);
		emit(in_obj->words[i], line);
	}
	delete in_obj;
}

EmitType* MapReduceInvertIndex::reduce(int64_t emit_key, EmitType* _a, EmitType* _b)
{
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

DocumentBatch::~DocumentBatch()
{

}

Document::Document(int b, int e, int docid)
{
	id = docid;
	m_b = b;
	m_e = e;       
}

void Document::fill()
{
	for (int i =m_b ; i<m_e; i++)
		words.push_back(i); 
}

Document::~Document() 
{
	words.clear();
}

bool DocumentBatch::end()
{
	return isend;
}

InputType* DocumentBatch::getNextInput()
{
	Document *doc_doc = m_docs.front();
	doc_doc->fill();
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
	
//	return 0;
	MapReduceInvertIndex *MR = new MapReduceInvertIndex();
	hThreadPool *pool = new hThreadPool(10);
	mr_disp = new MRBatchDispatcher(MR, 
									new InvertLineDumper,
									pool,
									boost::bind(&onAllReducesFinished));

	std::shared_ptr< std::vector<BatchAccessor*> > 
				batches (new std::vector<BatchAccessor*>);

	std::vector<Document*> docs;
	
	const int input_size = 100000;
	
	for (int i = 0; i<=input_size; i++)
	{
		Document *doc = new Document(i, i+2000, i);
		docs.push_back( doc );
		if (i%(input_size/10) ==0)
		{
			DocumentBatch *batch = new DocumentBatch(docs);
			docs.clear();
			batches->push_back(batch);
		}
	}
	
	start_time = time(0);
	pool->run();
	mr_disp->proceedBatches(batches);
	
	pool->join();

	return 0;
}
