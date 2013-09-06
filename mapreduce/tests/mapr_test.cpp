/* 
 * File:   mapr_test.cpp
 * Author: phrk
 *
 * Created on August 24, 2013, 1:58 AM
 */

#include <cstdlib>
#include "../core/mapreduce.h"
#include "../core/MapReduceDispatcher.h"

using namespace std;

class Document : public InputType
{
public:
    int64_t id;
    std::vector<int64_t> words;
};

class InvertLine : public EmitType
{
public:
    std::vector<int64_t> pages;
};

class MapReduceInvertIndex : public MapReduce
{
public:
    
    MapReduceInvertIndex(std::string job_name, std::string node_name) :
        MapReduce(job_name, node_name)
    {
        
    }
    
    virtual void map(InputType &object)
    {
        Document& in_obj = ( Document& ) object ; 
        for (int i = 0; i<in_obj.words.size(); i++)
        {
            InvertLine *line = new InvertLine;
            line->pages.push_back(in_obj.id);
            emit(in_obj.words[i], line);
        }
        
    }
    
    virtual EmitType* reduce(int64_t emit_key, EmitType* _a, EmitType* _b)
    {
        //std::vector<InvertLine*> *emits = (std::vector<InvertLine*>*) emits_;

        InvertLine *a = (InvertLine*) _a;
        InvertLine *b = (InvertLine*) _b;
        
        for (int i = 0; i<  b->pages.size(); i++)
        {
            a->pages.push_back(b->pages[i]);
        }
        
        delete b;
        return a;
    }
};

class DocumentBatch : public BatchAccessor
{
    bool isend;
    Document doc;
public:
    
    DocumentBatch(int b, int e, int docid)
    {
        isend = 0;
        doc.id = docid;
        for (int i =b ; i<e; i++)
        doc.words.push_back(i);        
    }
    
    virtual bool end()
    {
        return isend;
    }
    
    virtual InputType& getNextInput()
    {
        isend = 1;
        return doc;
    }
};

int main(int argc, char** argv) 
{
    MapReduceInvertIndex *MR = new MapReduceInvertIndex("InvertIndex", "localhost");
    
    MapReduceDispatcher *mr_disp = new MapReduceDispatcher(MR);
    DocumentBatch *batch = new DocumentBatch(0, 9, 42);
    DocumentBatch *batch2 = new DocumentBatch(5, 12, 13);
    BatchMapper* mapper = mr_disp->proceed(batch);
    mr_disp->proceed(batch2);
    
    //std::tr1::unordered_map<int64_t, std::vector<EmitType*>* > res_hash = mapper->
    
    EmitQueueHash* hash = mr_disp->emit_queue_hash;
    
    EmitQueueHash::iterator it = hash->begin();
    while (it != hash->end())
    {
        std::cout << it->first << ": ";
        EmitQueue::iterator vit = it->second->begin();
        while (vit != it->second->end())
        {
            InvertLine *line = (InvertLine*)it->second->at(0);
            std::cout << " " << line->pages[0] << ", ";
            vit++;
        }
        std::cout << std::endl;
        it++;
    }
    
    return 0;
}

