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

// if handled dont keep state in cache
InputState* HdividerStatesCache::getState(int64_t input_id)
{
    pthread_mutex_lock(&mutex);
    pthread_mutex_unlock(&mutex);
    
    tr1::unordered_map<int64_t, InputState*>::iterator it = cache->find(input_id);
    if (it!=cache->end())
    {
        if (it->second->handled)
        {
            InputState *state = new InputState(*it->second);
            delete it->second;
            cache->erase(it);
            return state;
        }
        else
        {
            return new InputState(*it->second);
        }
    }
    else
    {
        InputState *state = mongo_accessor->getState(input_id);
        if (state->handled)
        {
            return state;
        }
        else
        {
            cache->insert(pair<int64_t, InputState*>(input_id, state));
            return new InputState(*state);
        }
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
        if (state->handled)
        {
            delete it->second;
            cache->erase(it);
        }
        else
        {
            delete it->second;
            it->second = new InputState(*state);
        }
    }
    else
    {
        if (!state->handled)
        {
                InputState *new_state = new InputState(*state);
                cache->insert(pair<int64_t, InputState*>(state->id, new_state));
        }
    } 
}

void HdividerStatesCache::resetState()
{
    cache->clear();
    mongo_accessor->resetState();
}