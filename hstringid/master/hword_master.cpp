#include "hword_master.h"
#include <unistd.h>

using namespace std;

tr1::unordered_map<string, int64_t>* HwordDbInteractorStub::getIds()
{
    return new tr1::unordered_map<string, int64_t>;
}

void HwordDbInteractorStub::savePair(string word, int64_t id)
{

}

int64_t HwordDbInteractorStub::getId(string word)
{
    throw new string ("HwordDbInteractorStub::getId undefined ");
}

void *start_write_thread(void *a)
{
HwordMaster *hword_master = (HwordMaster*)a;
hword_master->writeAsyncThread();
}

HwordMaster::HwordMaster(HwordDbAccessor *db_int, HwordDbAccessor *db_write, bool cache_state)
{
    pthread_mutex_init(&lock, 0);
    pthread_mutex_init(&write_lock, 0);
    pthread_mutex_lock(&lock);
    this->cache_state = cache_state;
    hits = 0;
    reqs = 0;
    this->db_int = db_int;
    this->db_write = db_write;
    max_id = 0;
    global_cache = db_int->getIds();

    if (cache_state == CACHE_DISABLED)
    {
        max_id = global_cache->size();
        global_cache->clear();
        delete global_cache;
    }

    pthread_create(&write_th, NULL, start_write_thread, this);

    pthread_mutex_unlock(&lock);
}

HwordMaster::~HwordMaster()
{
    delete db_int;
    delete db_write;
    if (cache_state == CACHE_ENABLED)
    {
            delete global_cache;
    }
}

void HwordMaster::writeAsyncThread()
{
    bool wait = 0;
    while (1)
    {
        if (wait)
        {
            sleep(1);
        }

        pthread_mutex_lock(&write_lock);

        if (write_queue.size()==0)
        {
            wait = 1;
            pthread_mutex_unlock(&write_lock);
            continue;
        }
        else 
        {
            wait = 0;
        }

        pair<string, int64_t> hstringid =  write_queue.front();
        db_write->savePair(hstringid.first, hstringid.second);
        write_queue.pop();
        pthread_mutex_unlock(&write_lock);

    }
}

void HwordMaster::pushWrite(string word, int64_t id)
{
    pthread_mutex_lock(&write_lock);
    write_queue.push(pair<string, int64_t> (word, id));
    pthread_mutex_unlock(&write_lock);
}

int64_t HwordMaster::getId(string word)
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
            //db_int->savePair(word, id);
            pushWrite(word, id);
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
            //db_int->savePair(word, id);
            pushWrite(word, id);
            max_id++;
        }
    }

    pthread_mutex_unlock(&lock);
    return id;
}

pair<int,int> HwordMaster::getStat()
{
    return pair<int,int>(hits, reqs);
}

HwordMasterIfsSimpleCaller::HwordMasterIfsSimpleCaller (HwordMaster *master)
{
    this->master = master;
}

int64_t HwordMasterIfsSimpleCaller::getId(string word)
{
    return master->getId(word);
}