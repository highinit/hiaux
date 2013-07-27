#ifndef HWORD_NODE
#define HWORD_NODE

#include <tr1/unordered_map>
#include <string>

#include "../master/hword_master.h"

using namespace std;

#define CACHE_PRELOAD 1
#define CACHE_DONT_PRELOAD 0

class HwordNode
{
    HwordDbAccessor *db_int;
    tr1::unordered_map<string, int64_t> *local_cache;
    bool cache_state;
    bool cache_preload;
    HwordMasterIfs *master;
    int reqs;
    int hits;
    pthread_mutex_t lock;
protected:
    
    HwordNode()
    {
        cache_state = CACHE_DISABLED;
    }
    
public:
    
    
    
    HwordNode(HwordMasterIfs *master, HwordDbAccessor* db_int, bool cache_state, bool cache_preload)
    {
       // pthread_mutex_init(&lock, 0);
        this->cache_state = cache_state;
        this->master = master; 
        reqs = 0;
        hits = 0;
        this->db_int = db_int;
        if (cache_state == CACHE_ENABLED)
        {
                local_cache = new  tr1::unordered_map<string, int64_t>;
                if (cache_preload==CACHE_PRELOAD)
                local_cache = db_int->getIds();
        }
    }
    
    ~HwordNode()
    {
        //delete db_int;
        if (cache_state == CACHE_ENABLED)
        delete local_cache;
    }
    
    virtual int64_t getId(string word)
    {
        //pthread_mutex_lock(&lock);
        reqs++;
        if (cache_state == CACHE_ENABLED)
        {
            tr1::unordered_map<string, int64_t>::iterator it = local_cache->find(word);
            if (it!=local_cache->end())
            {
                hits++;
               // pthread_mutex_unlock(&lock);
                return it->second;
            }
            else
            {
                int64_t id = master->getId(word);
                local_cache->insert(pair<string, int64_t>(word, id));
               // pthread_mutex_unlock(&lock);
                return id;
            }
        }
        else
        {
           // pthread_mutex_unlock(&lock);
            return master->getId(word);
        }
    }
    
    pair<int,int> getStat()
    {
        return pair<int,int>(hits, reqs);
    }
};

#endif