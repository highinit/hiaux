#include "hdivider_state_accessor_cache.h"

void *hdivider_state_write_db(void* a)
{
    HdividerStatesCache *cache = (HdividerStatesCache*) a;
    cache->startWriteThread();
}


HdividerStatesCache::HdividerStatesCache (HdividerMongoStateAccessor *db)
{
    pthread_mutex_init(&queue_lock, 0);
    pthread_mutex_init(&mutex, 0);
    pthread_mutex_lock(&mutex);
    this->db = db;
    this->cache = db->getAllStates();
    queue_size = 0;
    nomore = 0;
    write_queue = new queue<InputState*>;
    pthread_create(&write_th, NULL, hdivider_state_write_db, this);
    pthread_mutex_unlock(&mutex);
}

HdividerStatesCache::~HdividerStatesCache()
{
    delete this->cache;
}

void HdividerStatesCache::startWriteThread()
{
    
//    cout << "startWriteThread" << endl;
    bool wait = 0;
    while (1)
    {
        if (wait)
        {
            sleep(1);
        }

        pthread_mutex_lock(&queue_lock);

        if (queue_size==0)
        {
            if (nomore)
            {
                break;
            }
            wait = 1;
            pthread_mutex_unlock(&queue_lock);
            continue;
        }
        else 
        {
            wait = 0;
        }
 
        InputState *state =  write_queue->front();

        db->saveState(state);
        write_queue->pop();
        queue_size--;
        delete state;
        pthread_mutex_unlock(&queue_lock);

    }
}

void HdividerStatesCache::pushWrite(const InputState *state)
{
    pthread_mutex_lock(&queue_lock);
    queue_size++;
    write_queue->push(new InputState(state));
    pthread_mutex_unlock(&queue_lock);
}

// if handled dont keep state in cache
InputState* HdividerStatesCache::getState(int64_t input_id)
{
  //  cout << "getState" << endl;
    pthread_mutex_lock(&mutex);
    
    tr1::unordered_map<int64_t, InputState*>::iterator it = cache->find(input_id);
    if (it!=cache->end())
    {
      /*  if (it->second->handled)
        {
    //        cout << "1 (it->second->handled" << endl;
            InputState *state = new InputState(*it->second);
            //delete it->second;
            //cache->erase(it);
            
            pthread_mutex_unlock(&mutex);
            return state;
        }
       else*/
        {
            pthread_mutex_unlock(&mutex);
       //     cout << "2 return new InputState(*it->second);" << endl;
            return new InputState(it->second);
        }
    }
    else
    {
        //InputState *state = db_read->getState(input_id);
        InputState *state = new InputState(input_id, "", 0);
        pushWrite(state);
      /*  if (state->handled)
        {
            pthread_mutex_unlock(&mutex);
            return state;
        }
        else*/
        {
            cache->insert(pair<int64_t, InputState*>(input_id, state));
            pthread_mutex_unlock(&mutex);
            return new InputState(state);
        }
    }
}

tr1::unordered_map<int64_t, InputState*>* HdividerStatesCache::getAllStates()
{
    return this->cache;
}

void HdividerStatesCache::saveState(const InputState *state)
{
 //   cout << "saveState" << endl;
    pushWrite(state);
    //mongo_accessor->saveState(state);
    pthread_mutex_lock(&mutex);
    tr1::unordered_map<int64_t, InputState*>::iterator it = cache->find(state->id);
    if (it!=cache->end())
    {
      //  if (state->handled)
        {
            //delete it->second;
            //cache->erase(it);
        }
       // else
        {
            delete it->second;
            it->second = new InputState(*state);
        }
    }
    else
    {
       // if (!state->handled)
        {
                InputState *new_state = new InputState(*state);
                cache->insert(pair<int64_t, InputState*>(state->id, new_state));
        }
    } 
    pthread_mutex_unlock(&mutex);
}

void HdividerStatesCache::resetState()
{
    pthread_mutex_lock(&mutex);
    cache->clear();
    db->resetState();
    pthread_mutex_unlock(&mutex);
}

 void HdividerStatesCache::join()
 {
     pthread_join(write_th, 0);
 }
 
 void HdividerStatesCache::setNoMore()
 {
     nomore = 1;
 }