/* 
 * File:   mapr_test.cpp
 * Author: phrk
 *
 * Created on August 24, 2013, 1:58 AM
 */

#include <cstdlib>
#include "../core/mapreduce.h"
#include "../core/MRNodeDispatcher.h"

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
    
    virtual void dump(std::string filename)
    {
      
    }
    
    virtual void restore(std::string filename)
    {
        
    }
};

class MapReduceInvertIndex : public MapReduce
{
public:
    
    MapReduceInvertIndex(std::string job_name, std::string node_name) :
        MapReduce(job_name, node_name)
    {
        
    }
    
    virtual void map(const InputType &object)
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
        InvertLine *a = (InvertLine*) _a;
        InvertLine *b = (InvertLine*) _b;
        
        for (int i = 0; i<  b->pages.size(); i++)
        {
            a->pages.push_back(b->pages[i]);
        }
        
        delete b;
        return a;
    }
    
    virtual void finilize(EmitType* result) 
    {
    
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

void onAllBatchesFinished(EmitQueueHash *queue_hash)
{
    std::cout << "onAllBatchesFinished" << std::endl;
}

int main(int argc, char** argv) 
{
    MapReduceInvertIndex *MR = new MapReduceInvertIndex("InvertIndex", "localhost");
    hThreadPool *pool = new hThreadPool(4);
    MRNodeDispatcher *mr_disp = new MRNodeDispatcher(MR, pool, boost::bind(&onAllBatchesFinished, _1));
    DocumentBatch *batch = new DocumentBatch(0, 9, 42);
    DocumentBatch *batch2 = new DocumentBatch(5, 12, 13);
    std::vector<BatchAccessor*> batches;
    batches.push_back(batch);
    batches.push_back(batch2);
    
    mr_disp->proceedBatches(batches);
    
    
    pool->run();
    sleep(1);
    
    //std::tr1::unordered_map<int64_t, std::vector<EmitType*>* > res_hash = mapper->
    
    EmitQueueHash* hash = mr_disp->emit_queue_hash;
    
    EmitQueueHash::iterator it = hash->begin();
    while (it != hash->end())
    {
        std::cout << "key:" << it->first << " emits: " << it->second->size() << "| ";
        
        while (it->second->size() != 0)
        {
            std:cout << ":(";
            InvertLine* line = (InvertLine*) it->second->front();
            it->second->pop();
            
            std::cout << line->pages.size() << ") "; 
            for (int i =0 ; i<line->pages.size(); i++)
            std::cout << " " << line->pages[i] << ", ";
        }
        std::cout << std::endl;
        it++;
    }
    
    MRStats stats = mr_disp->getStats();
    std::cout << "maps: " << stats.nmaps << std::endl;
    std::cout << "emits: " << stats.nemits << std::endl;
    std::cout << "reduces: " << stats.nreduces << std::endl;
    
    return 0;
}

