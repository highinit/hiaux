
#ifndef HWORD_MASTER
#define HWORD_MASTER

#include <tr1/unordered_map>
#include <queue>
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

void *start_write_thread(void *a);

class HwordMaster
{
    HwordDbAccessor *db_int;
    HwordDbAccessor *db_write;
    int64_t max_id;
    tr1::unordered_map<string, int64_t> *global_cache;
    pthread_mutex_t lock;
    int reqs;
    int hits;
    bool cache_state;
    
    pthread_t write_th;
    pthread_mutex_t write_lock;
    queue<pair<string, int64_t> > write_queue;
    
    void pushWrite(string word, int64_t id);
    
public:
    
    HwordMaster(HwordDbAccessor *db_int, HwordDbAccessor *db_write, bool cache_state);
    ~HwordMaster();
     
    void writeAsyncThread();
    
    //#remote
    virtual int64_t getId(string word);
    
    pair<int,int> getStat();
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