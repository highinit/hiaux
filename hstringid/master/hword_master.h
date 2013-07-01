
#ifndef HWORD_MASTER
#define HWORD_MASTER

#include <tr1/unordered_map>
#include <string>

using namespace std;

class HwordDbAccessor
{
public:
    virtual tr1::unordered_map<string, int64_t> *getIds() = 0;
    virtual void savePair(string word, int64_t id) = 0;
};

class HwordDbInteractorStub : public HwordDbAccessor
{
public:
    
    tr1::unordered_map<string, int64_t> *getIds()
    {
        return new tr1::unordered_map<string, int64_t>;
    }
    void savePair(string word, int64_t id)
    {

    }
};

class HwordMaster
{
    HwordDbAccessor *db_int;
    tr1::unordered_map<string, int64_t> *global_cache;
    pthread_mutex_t lock;
    int reqs;
    int hits;
    bool cache_enabled;
public:
    
    HwordMaster(HwordDbAccessor *db_int, bool cache_enabled)
    {
        pthread_mutex_init(&lock, 0);
        pthread_mutex_lock(&lock);
        this->cache_enabled = cache_enabled;
        hits = 0;
        reqs = 0;
        this->db_int = db_int;
        if (cache_enabled)
        {
                global_cache = db_int->getIds();
        }
        pthread_mutex_unlock(&lock);
    }
    
    ~HwordMaster()
    {
        delete db_int;
        delete global_cache;
    }
    
    //#remote
    virtual int64_t getId(string word)
    {
        reqs++;
        int64_t id;
        pthread_mutex_lock(&lock);
        tr1::unordered_map<string, int64_t>::iterator it = global_cache->find(word);
        if (it!=global_cache->end())
        {
            hits++;
            id = it->second;
        }
        else
        {
            id = global_cache->size();
            db_int->savePair(word, id);
            if (cache_enabled)
            {
                global_cache->insert(pair<string, int64_t>(word, id));
            } 
        }
        pthread_mutex_unlock(&lock);
        return id;
    }
    
    pair<int,int> getStat()
    {
        return pair<int,int>(hits, reqs);
    }
};


class HwordMasterIfs
{
public:
    virtual int64_t getId(string word) = 0;
};

class HwordMasterIfsSimpleCaller: public HwordMasterIfs
{
    HwordMaster *master;
    public:
    HwordMasterIfsSimpleCaller (HwordMaster *master)
    {
        this->master = master;
    }
    
    virtual int64_t getId(string word)
    {
        return master->getId(word);
    }
};

#endif