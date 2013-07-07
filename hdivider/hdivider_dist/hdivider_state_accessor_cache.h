/* 
 * File:   hdivider_state_accessor_cache.h
 * Author: phrk
 *
 * Created on June 22, 2013, 4:52 PM
 */

#ifndef HDIVIDER_STATE_ACCESSOR_CACHE_H
#define	HDIVIDER_STATE_ACCESSOR_CACHE_H

#include "hdivider_mongo_accessors.h"

class HdividerStatesCache : public HdividerStateAccessor
{
    tr1::unordered_map<int64_t, InputState*>* cache;
    HdividerMongoStateAccessor *db;
    pthread_mutex_t mutex;
    
    pthread_t write_th;
    pthread_mutex_t queue_lock;
    queue<InputState*> *write_queue;
    bool nomore;
    int queue_size;
    void pushWrite(const InputState *state);
    
public:
        
    HdividerStatesCache (HdividerMongoStateAccessor *db);
    ~HdividerStatesCache();
    
    void startWriteThread();
    
    void join();
    void setNoMore();
    
    virtual InputState*                                 getState(int64_t input_id);
    tr1::unordered_map<int64_t, InputState*>*           getAllStates();
    virtual void                                        saveState(const InputState *state);
    virtual void                                        resetState();
};


#endif	/* HDIVIDER_STATE_ACCESSOR_CACHE_H */

