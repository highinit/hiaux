
#ifndef HWORD_MASTER
#define HWORD_MASTER

#include <tr1/unordered_map>
#include <string>

using namespace std;

#define CACHE_ENABLED 1
#define CACHE_DISABLED 0

class HwordDbAccessor
{
public:
    virtual tr1::unordered_map<string, int64_t> *getIds() = 0;
    virtual void savePair(string word, int64_t id) = 0;
    virtual int64_t getId(string word) = 0;
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
    
    int64_t getId(string word)
    {
        throw new string ("HwordDbInteractorStub::getId undefined ");
    }
};

class HwordMaster
{
    HwordDbAccessor *db_int;
    int64_t max_id;
    tr1::unordered_map<string, int64_t> *global_cache;
    pthread_mutex_t lock;
    int reqs;
    int hits;
    bool cache_state;
public:
    
    HwordMaster(HwordDbAccessor *db_int, bool cache_state)
    {
        pthread_mutex_init(&lock, 0);
        pthread_mutex_lock(&lock);
        this->cache_state = cache_state;
        hits = 0;
        reqs = 0;
        this->db_int = db_int;
        max_id = 0;
        global_cache = db_int->getIds();
        
        if (cache_state == CACHE_DISABLED)
        {
            max_id = global_cache->size();
            global_cache->clear();
            delete global_cache;
        }
        
        pthread_mutex_unlock(&lock);
    }
    
    ~HwordMaster()
    {
        delete db_int;
        if (cache_state == CACHE_ENABLED)
        {
                delete global_cache;
        }
    }
    
    //#remote
    virtual int64_t getId(string word)
    {
        reqs++;
        int64_t id;
        pthread_mutex_lock(&lock);
        if (cache_state == CACHE_ENABLED)
        {
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
                global_cache->insert(pair<string, int64_t>(word, id));
            }
        }
        else
        {
            try
            {
                id = db_int->getId(word);
                //cout << "got id " << id << endl;
            } catch (string *s)
            {
                delete s;
                id = max_id;
                db_int->savePair(word, id);
                max_id++;
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