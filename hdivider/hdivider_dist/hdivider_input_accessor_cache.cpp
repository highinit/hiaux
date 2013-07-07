#include "hdivider_input_accessor_cache.h"

void* hdivider_input_cache_thread(void *a)
{
    HdividerInputIdCache *cache = (HdividerInputIdCache*)a;
    cache->start_caching();
}       

void HdividerInputIdCache::start_caching()
{
   // cout << "caching thread started\n";
    while (1)
    { 
        pthread_mutex_lock(&cacher_lock);
        
        pthread_cond_destroy(&cacher_cond);
        pthread_cond_init(&cacher_cond, NULL);
        
        while (cacher_wait)
        {
            //cout << "cacher waiting to unlock cacher_wait=" << cacher_wait << endl;
            pthread_cond_wait(&cacher_cond, &cacher_lock);
        }
        
        if (!mongo_it->end())
        {
           vector<int64_t> new_ids;
           
           //cout << "reading from db\n";
           while (new_ids.size() < cache_size && !mongo_it->end())
           {
               new_ids.push_back(mongo_it->value());
               mongo_it->getNext();
           }
           
           //cout << "got from ids db. waiting to write to cached_ids\n";
           
           pthread_mutex_lock(&ids_lock);
           vector<int64_t>::iterator it = new_ids.begin();
           while (it!=new_ids.end())
           {
               cached_ids.push(*it);
               it++;
           }
           new_ids.clear();
           pthread_mutex_unlock(&ids_lock);
        }
        else
        {
            //cout << "mongo_it END\n";
            cacher_wait = 1;
            pthread_mutex_unlock(&cacher_lock);
            break;
        }
        
        cacher_wait = 1;
        pthread_mutex_unlock(&cacher_lock);
    }
}

void HdividerInputIdCache::cache_became_smaller()
{
    pthread_mutex_lock(&cacher_lock);
    pthread_mutex_lock(&ids_lock);
    if (cached_ids.size()< cache_size /2)
    {
        //cout << "unlocking cacher \n";

        cacher_wait = 0;
        pthread_cond_signal(&cacher_cond);
        
    }
    pthread_mutex_unlock(&ids_lock);
    pthread_mutex_unlock(&cacher_lock);
}

HdividerInputIdCache::HdividerInputIdCache(string ip, int port, string db_name, string coll_name, string login, string pass, int cache_size)
{
   // cout << "HdividerInputIdCache::HdividerInputIdCache " << endl;
    isend = 0;
    pthread_mutex_init(&ids_lock, 0);
    pthread_mutex_init(&cacher_lock, 0);
    pthread_cond_init(&cacher_cond, NULL);
    this->mongo_it = new HdividerMongoInputIdIt(ip, port, db_name, coll_name, login, pass);
    this->cache_size = cache_size;
    cacher_wait = 1;
    pthread_create(th, NULL, hdivider_input_cache_thread, (void*)this);
    setFirst();
    
}

int HdividerInputIdCache::size()
{
    return mongo_it->size();
}

void HdividerInputIdCache::setFirst()
{
    //cout << "setFirst\n";
    mongo_it->setFirst();
    getNext();
    isend = mongo_it->end();
    //cache_became_smaller();
}

void HdividerInputIdCache::getNext()
{
    //cout << " cache size " << cached_ids.size() << " ";
    pthread_mutex_lock(&ids_lock);
    if (cached_ids.size()==0)
    {
        pthread_mutex_unlock(&ids_lock);
        
       // cache_became_smaller();
        
        while (1)
        {      
            if (mongo_it->end())
            {
                isend = 1;
                return;
            }
            
            cache_became_smaller();
            
            sleep(1);
            
            pthread_mutex_lock(&ids_lock);
            if (cached_ids.size()!=0)
            {
                pthread_mutex_unlock(&ids_lock);
                break;
            }
            pthread_mutex_unlock(&ids_lock);
        }
        
        pthread_mutex_lock(&ids_lock);
        id_value = cached_ids.front();
        cached_ids.pop();
        //cout << "pop \n";
        pthread_mutex_unlock(&ids_lock);
    }
    else
    {
        id_value = cached_ids.front();
        cached_ids.pop();
        //cout << "pop \n";
        pthread_mutex_unlock(&ids_lock);
        cache_became_smaller();
    }
}

int64_t HdividerInputIdCache::value()
{
    return id_value;
}

bool HdividerInputIdCache::end()
{
    return isend;
}
