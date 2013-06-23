#include "hdivider_state_accessor_cache.h"

HdividerStatesCache::HdividerStatesCache (HdividerMongoStateAccessor *mongo_accessor)
{
    pthread_mutex_init(&mutex, 0);
    pthread_mutex_lock(&mutex);
    this->mongo_accessor = mongo_accessor;
    this->cache = mongo_accessor->getAllStates();
    pthread_mutex_unlock(&mutex);
}

HdividerStatesCache::~HdividerStatesCache()
{
    delete this->cache;
}

InputState* HdividerStatesCache::getState(int64_t input_id)
{
    pthread_mutex_lock(&mutex);
    pthread_mutex_unlock(&mutex);
    
    tr1::unordered_map<int64_t, InputState*>::iterator it = cache->find(input_id);
    if (it!=cache->end())
    {
        return new InputState(*it->second);
    }
    else
    {
        InputState *state = mongo_accessor->getState(input_id);
        cache->insert(pair<int64_t, InputState*>(input_id, state));
        return new InputState(*state);
    }
}

tr1::unordered_map<int64_t, InputState*>* HdividerStatesCache::getAllStates()
{
    return this->cache;
}

void HdividerStatesCache::saveState(const InputState *state)
{
    mongo_accessor->saveState(state);
    tr1::unordered_map<int64_t, InputState*>::iterator it = cache->find(state->id);
    if (it!=cache->end())
    {
        delete it->second;
        it->second = new InputState(*state);
    }
    else
    {
        InputState *new_state = new InputState(*state);
        cache->insert(pair<int64_t, InputState*>(state->id, new_state));
    } 
}

void HdividerStatesCache::resetState()
{
    cache->clear();
    mongo_accessor->resetState();
}