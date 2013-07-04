#ifndef HWORD_NODE
#define HWORD_NODE

#include <tr1/unordered_map>
#include <string>

#include "../master/hword_master.h"

using namespace std;

class HwordNode
{
    //HwordDbAccessor *db_int;
    tr1::unordered_map<string, int64_t> *local_cache;
    bool cache_state;
    HwordMasterIfs *master;
    int reqs;
    int hits;
public:
    
    HwordNode()
    {
        cache_state = CACHE_DISABLED;
    }
    
    HwordNode(HwordMasterIfs *master, bool cache_state)
    {
        this->cache_state = cache_state;
        this->master = master; 
        reqs = 0;
        hits = 0;
        //this->db_int = db_int;
        if (cache_state == CACHE_ENABLED)
        local_cache = new  tr1::unordered_map<string, int64_t>;
        //local_cache = db_int->getIds();
    }
    
    ~HwordNode()
    {
        //delete db_int;
        if (cache_state == CACHE_ENABLED)
        delete local_cache;
    }
    
    virtual int64_t getId(string word)
    {
        reqs++;
        if (cache_state == CACHE_ENABLED)
        {
            tr1::unordered_map<string, int64_t>::iterator it = local_cache->find(word);
            if (it!=local_cache->end())
            {
                hits++;
                return it->second;
            }
            else
            {
                int64_t id = master->getId(word);
                local_cache->insert(pair<string, int64_t>(word, id));
                return id;
            }
        }
        else
        {
            return master->getId(word);
        }
    }
    
    pair<int,int> getStat()
    {
        return pair<int,int>(hits, reqs);
    }
};

#endif