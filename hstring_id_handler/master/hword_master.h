
#ifndef HWORD_MASTER
#define HWORD_MASTER

#include <map>
#include <string>

using namespace std;

class HwordDbAccessor
{
public:
    virtual map<string, int64_t> *getIds() = 0;
    virtual void savePair(string word, int64_t id) = 0;
};

class HwordDbInteractorStub : public HwordDbAccessor
{
public:
    
    map<string, int64_t> *getIds()
    {
        return new map<string, int64_t>;
    }
    void savePair(string word, int64_t id)
    {

    }
};

class HwordMaster
{
    HwordDbAccessor *db_int;
    map<string, int64_t> *global_cache;
    pthread_mutex_t lock;
    int reqs;
    int hits;
public:
    
    HwordMaster(HwordDbAccessor *db_int)
    {
        pthread_mutex_init(&lock, 0);
        pthread_mutex_lock(&lock);
        hits = 0;
        reqs = 0;
        this->db_int = db_int;
        global_cache = db_int->getIds();
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
        map<string, int64_t>::iterator it = global_cache->find(word);
        if (it!=global_cache->end())
        {
            hits++;
            id = it->second;
        }
        else
        {
            id = global_cache->size();
            global_cache->insert(pair<string, int64_t>(word, id));
            db_int->savePair(word, id);
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