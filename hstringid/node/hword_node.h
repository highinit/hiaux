#ifndef HWORD_NODE
#define HWORD_NODE

#include <tr1/unordered_map>
#include <string>

#include "../master/hword_master.h"

using namespace std;

class HwordNode
{
    HwordDbAccessor *db_int;
    tr1::unordered_map<string, int64_t> *local_cache;
    HwordMasterIfs *master;
    int reqs;
    int hits;
public:
    
    HwordNode(HwordMasterIfs *master, HwordDbAccessor *db_int)
    {
        this->master = master; 
        reqs = 0;
        hits = 0;
        this->db_int = db_int;
        local_cache = db_int->getIds();
    }
    
    ~HwordNode()
    {
        delete db_int;
        delete local_cache;
    }
    
    int64_t getId(string word)
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
            int64_t id = master->getId(word);
            local_cache->insert(pair<string, int64_t>(word, id));
            return id;
        }
    }
    
    pair<int,int> getStat()
    {
        return pair<int,int>(hits, reqs);
    }
};

#endif