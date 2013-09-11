
#ifndef HWORD_MASTER
#define HWORD_MASTER

#include <tr1/unordered_map>
#include <queue>
#include <string>
#include <stdint.h>

#define CACHE_ENABLED 1
#define CACHE_DISABLED 0

class HwordDbAccessor
{
public:
    virtual std::tr1::unordered_map<std::string, int64_t> *getIds() = 0;
    virtual void savePair(std::string word, int64_t id) = 0;
    virtual int64_t getId(std::string word) = 0;
};

class HwordDbInteractorStub : public HwordDbAccessor
{
public:
    
    std::tr1::unordered_map<std::string, int64_t> *getIds();
    void savePair(std::string word, int64_t id);
    int64_t getId(std::string word);
};

void *start_write_thread(void *a);

class HwordMaster
{
    HwordDbAccessor *db_int;
    HwordDbAccessor *db_write;
    int64_t max_id;
    std::tr1::unordered_map<std::string, int64_t> *global_cache;
    pthread_mutex_t lock;
    int reqs;
    int hits;
    bool cache_state;
    
    pthread_t write_th;
    pthread_mutex_t write_lock;
    std::queue<std::pair<std::string, int64_t> > write_queue;
    
    void pushWrite(std::string word, int64_t id);
    
public:
    
    HwordMaster(HwordDbAccessor *db_int, HwordDbAccessor *db_write, bool cache_state);
    ~HwordMaster();
     
    void writeAsyncThread();
    
    //#remote
    virtual int64_t getId(std::string word);
    
    virtual void mergeIds(std::vector<int64_t> ids);
    
    std::pair<int,int> getStat();
};


class HwordMasterIfs
{
public:
    virtual int64_t getId(std::string word) = 0;
};

class HwordMasterIfsSimpleCaller: public HwordMasterIfs
{
    HwordMaster *master;
public:
    
    HwordMasterIfsSimpleCaller (HwordMaster *master);
    
    virtual int64_t getId(std::string word);
};

#endif