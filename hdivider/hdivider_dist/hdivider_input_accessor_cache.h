/* 
 * File:   hdivider_input_accessor_cache.h
 * Author: phrk
 *
 * Created on June 29, 2013, 3:40 PM
 */

#ifndef HDIVIDER_INPUT_ACCESSOR_CACHE_H
#define	HDIVIDER_INPUT_ACCESSOR_CACHE_H

#include "hdivider_mongo_accessors.h"

class HdividerInputIdCache: public  HdividerInputIdIt
{
    int64_t id_value;
    bool isend;
    
    HdividerMongoInputIdIt *mongo_it;
    pthread_t th[1];
    
    int cache_size;
    
    queue<int64_t> cached_ids;
    pthread_mutex_t ids_lock;
    
    pthread_cond_t cacher_cond;
    bool cacher_wait;
    pthread_mutex_t cacher_lock;
    
    void cache_became_smaller();
    
    HdividerInputIdCache()  { }
    
    public:
        
    HdividerInputIdCache(string ip, int port, string db_name, string coll_name, string login, string pass, int cache_size);
    
    void start_caching();
    
    virtual int size();
    virtual void setFirst();
    virtual void getNext();
    virtual int64_t value();
    virtual bool end();
};

#endif	/* HDIVIDER_INPUT_ACCESSOR_CACHE_H */

