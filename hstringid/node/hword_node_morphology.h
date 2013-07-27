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
    HwordDbAccessor* db_int;
    bool cache_preload;
    tr1::unordered_map<string, int64_t> *local_cache;
    HwordMasterIfs *master;
    HwordStemmer *stemmer;
    int reqs;
    int hits;
    
    pthread_mutex_t lock;
public:
    
    HwordNodeMorphology(HwordMasterIfs *master, HwordDbAccessor* db_int, HwordStemmer *stemmer, bool cache_preload)
    {
        //pthread_mutex_init(&lock, 0);
        this->cache_preload = cache_preload;
        this->master = master; 
        reqs = 0;
        hits = 0;
        local_cache = new  tr1::unordered_map<string, int64_t>;
        this->stemmer = stemmer;
        if (cache_preload)
        {
            local_cache = db_int->getIds();
        }
    }
    
    ~HwordNodeMorphology()
    {
        delete local_cache;
    }
    
    virtual int64_t getId(string word)
    {
        //pthread_mutex_lock(&lock);
        reqs++;
       
        tr1::unordered_map<string, int64_t>::iterator it = local_cache->find(word);
        if (it!=local_cache->end())
        {
            hits++;
            //pthread_mutex_unlock(&lock);
            return it->second;
        }
        else
        {
            string word_norm = stemmer->stemWord(word);
            tr1::unordered_map<string, int64_t>::iterator it = local_cache->find(word_norm);
            if (it!=local_cache->end())
            {
                local_cache->insert(pair<string, int64_t>(word, it->second));
                int64_t id = it->second;
                //pthread_mutex_unlock(&lock);
                return id;
            }
            else
            {
                //string word_norm = word;
                
                int64_t id = master->getId(word_norm);

                local_cache->insert(pair<string, int64_t>(word, id));
                local_cache->insert(pair<string, int64_t>(word_norm, id));
                //pthread_mutex_unlock(&lock);
                return id;
            }
        }   
    }
    
    pair<int,int> getStat()
    {
        return pair<int,int>(hits, reqs);
    }
};

#endif	/* HWORD_NODE_MORPHOLOGY_H */

