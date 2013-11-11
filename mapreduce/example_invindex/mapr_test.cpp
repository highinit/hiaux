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

#include "mapr_test.h"
#include <iostream>

#include "invertline.pb.h"

using namespace std;

MapReduceInvertIndex::MapReduceInvertIndex() 
{

}

void MapReduceInvertIndex::map(InputType* object)
{	
	Document *in_obj = (Document *) object ;

	for (int i = 0; i<in_obj->words.size(); i++)
	{
		InvertLine *line = new InvertLine();
		line->pages.push_back(in_obj->id);
		emit(in_obj->words[i], line);
	}
	delete in_obj;
}

EmitType* MapReduceInvertIndex::reduce(uint64_t key, EmitType* _a, EmitType* _b)
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

MapReduce *MapReduceInvertIndex::create()
{
	return new MapReduceInvertIndex(*this);
}

std::string MapReduceInvertIndex::dumpEmit(EmitType *emit)
{
	InvertLine *line = (InvertLine*) emit;
	mapr_test::InvertLine pb_line;
	
	for (int i = 0; i<line->pages.size(); i++)
		pb_line.add_pages(line->pages[i]);

	std::string ret =  pb_line.SerializeAsString();
	pb_line.Clear();
	return ret;
}

EmitType* MapReduceInvertIndex::restoreEmit(std::string dumped)
{
	InvertLine *line = new InvertLine();
	mapr_test::InvertLine pb_line;
	pb_line.ParseFromString(dumped);
	
	for (int i = 0; i<pb_line.pages_size(); i++)
	{
		line->pages.push_back(pb_line.pages(i));
	}
	pb_line.Clear();
	return (EmitType*) line;
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
	if (m_docs.size()==0) isend = 1;
	return  doc;
}

