/* 
 * File:   hword_node_morphology.h
 * Author: phrk
 *
 * Created on July 4, 2013, 4:53 PM
 */

#ifndef HWORD_NODE_MORPHOLOGY_H
#define	HWORD_NODE_MORPHOLOGY_H

#include <tr1/unordered_map>
#include <string>

#include "../master/hword_master.h"

using namespace std;

class HwordStemmer
{
public:
    virtual string stemWord(string word) = 0; 
};

class HwordNodeMorphology: public HwordNode
{
    tr1::unordered_map<string, int64_t> *local_cache;
    HwordMasterIfs *master;
    HwordStemmer *stemmer;
    int reqs;
    int hits;
public:
    
    HwordNodeMorphology(HwordMasterIfs *master, HwordStemmer *stemmer)
    {
        this->master = master; 
        reqs = 0;
        hits = 0;
        local_cache = new  tr1::unordered_map<string, int64_t>;
        this->stemmer = stemmer;
    }
    
    ~HwordNodeMorphology()
    {
        delete local_cache;
    }
    
    virtual int64_t getId(string word)
    {
        reqs++;
       
        tr1::unordered_map<string, int64_t>::iterator it = local_cache->find(word);
        if (it!=local_cache->end())
        {
            hits++;
            return it->second;
        }
        else
        {
            //string word_norm = word;
            string word_norm = stemmer->stemWord(word);
            int64_t id = master->getId(word_norm);
            
            local_cache->insert(pair<string, int64_t>(word, id));
            local_cache->insert(pair<string, int64_t>(word_norm, id));
            return id;
        }   
    }
    
    pair<int,int> getStat()
    {
        return pair<int,int>(hits, reqs);
    }
};

#endif	/* HWORD_NODE_MORPHOLOGY_H */

