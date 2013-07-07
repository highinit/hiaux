#include "../include/hdivider_watcher.h"

InputState::InputState()
{
    handled = 0;
    locked_by = "";
}

InputState::InputState(const InputState &state)
{
    id = state.id;
    locked_by = state.locked_by;
    handled = state.handled;
}

InputState::InputState(const InputState *state)
{
    id = state->id;
    locked_by = state->locked_by;
    handled = state->handled;
}

InputState::InputState(InputId id, string locked_by, bool handled)
{
    this->id = id;
    this->locked_by = locked_by;
    this->handled = handled;
}

ResultLockInfo::ResultLockInfo(pthread_mutex_t* lock, string worker_id)
{
    this->lock = lock;
    this->worker_id = worker_id;
}

ResultLockInfo::~ResultLockInfo()
{
    delete lock;
}

HdividerWatcher::HdividerWatcher()
{
    finished = 0;
    nhandled = 0;
    pthread_mutex_init(&get_mutex, 0);
    pthread_mutex_init(&put_mutex, 0);
    pthread_mutex_init(&result_locks_write_lock, 0);
}

HdividerWatcher::HdividerWatcher(HdividerInputIdIt *input_id_it, HdividerStateAccessor *state_accessor)
{
    pthread_mutex_init(&get_mutex, 0);
    pthread_mutex_init(&put_mutex, 0);
    pthread_mutex_init(&result_locks_write_lock, 0);
    this->input_id_it = input_id_it;
    this->state_accessor = state_accessor;
    finished = 0;
    nhandled = 0;
}

HdividerWatcher::~HdividerWatcher()
{
    tr1::unordered_map<ResultId, ResultLockInfo*>::iterator it = result_locks.begin();
    
    while (it!=result_locks.end())
    {
        delete it->second;
        result_locks.erase(it);
        it++;
    }
    
//    delete input_id_it;
//    delete state_accessor;
}

void HdividerWatcher::start(int port)
{
    /*
    state_accessor->resetState();
    input_id_it->setFirst();
    while(!input_id_it->end())
    {
        InputState *state = new InputState (input_id_it->value(), "", 0);
        state_accessor->saveState( state );
        delete state;
        
        input_id_it->getNext();
    }   */
    // rpc server shit
}

void HdividerWatcher::resume(int port)
{
    // rpc server shit
}

void HdividerWatcher::handleFault(string worker_id)
{
    pthread_mutex_lock(&result_locks_write_lock);
    tr1::unordered_map<ResultId, ResultLockInfo*>::iterator it = result_locks.begin();
    while (it!=result_locks.end())
    {
        if (it->second->worker_id==worker_id)
        {
            pthread_mutex_unlock(it->second->lock);
        }
        it++;
    }   
    pthread_mutex_unlock(&result_locks_write_lock);
    
    
    pthread_mutex_lock(&get_mutex);
    input_id_it->setFirst();
    pthread_mutex_lock(&put_mutex);
    while (!input_id_it->end())
    {      
        InputState *state = state_accessor->getState(input_id_it->value());
        if (state->locked_by==worker_id && !state->handled)
        {
            state->locked_by = "";
            state_accessor->saveState(state);
        }
        delete state;   
        input_id_it->getNext();
    }
    pthread_mutex_unlock(&put_mutex);
    pthread_mutex_unlock(&get_mutex);
}

vector<int64_t> HdividerWatcher::getInput(int count, string worker_id)
{
    pthread_mutex_lock(&get_mutex);
    vector<int64_t> input_ids;

    int times_end = 0;
    
    while (input_ids.size()<count)
    {
        if (input_ids.size()+nhandled == input_id_it->size())
        {
            break;
        }
        
        if (input_id_it->end())
        {
            times_end++;
            if (input_ids.size()+nhandled==input_id_it->size())
            {
                break;
                //finished = 1;
            }
            input_id_it->setFirst();
        }
        
        if (times_end==2)
        {
            break;
        }
        
        if (finished)
        {
            break;
        }

        pthread_mutex_lock(&put_mutex);
        //cout << "getting state\n";
        int64_t id = input_id_it->value(); 
        InputState *state = state_accessor->getState(id);
        if (state->locked_by=="")
        {
            input_ids.push_back(state->id);
            state->locked_by = worker_id;
            state->handled = 0;
            //cout << "saving state\n";
            state_accessor->saveState(state);
        }
        pthread_mutex_unlock(&put_mutex);
        
        delete state;  
        //cout << "input next\n";
        input_id_it->getNext();
    }
    
    pthread_mutex_unlock(&get_mutex);
    
    return input_ids;
}

bool HdividerWatcher::isFinished()
{
    return finished;
}

void HdividerWatcher::setHandled(InputId input_id)
{
    pthread_mutex_lock(&put_mutex);
    InputState *state = state_accessor->getState(input_id);
    state->handled = 1;
    nhandled++;
    state_accessor->saveState(state);
    
    if (nhandled==input_id_it->size())
    {
        finished = 1;
    }
    
    delete state;
    pthread_mutex_unlock(&put_mutex);
}

int HdividerWatcher::isHandled(InputId input_id)
{
    pthread_mutex_lock(&put_mutex);
    InputState *state = state_accessor->getState(input_id);
    
    int handled = state->handled;
    
    delete state;
    pthread_mutex_unlock(&put_mutex);
    return handled;
}

void HdividerWatcher::lockResult(ResultId result_id, string worker_id)
{
    pthread_mutex_lock(&result_locks_write_lock);
 /*   char bf[25];
    sprintf(bf, "%d", (int)result_id);
    LOG("HdividerWatcher::lockResult "+worker_id+" "+bf);*/
    tr1::unordered_map<ResultId, ResultLockInfo*>::iterator it = result_locks.find(result_id);
    
    if (it!=result_locks.end())
    {
        pthread_mutex_t *lock = it->second->lock;
        pthread_mutex_unlock(&result_locks_write_lock);
        it->second->worker_id = string(worker_id);
        pthread_mutex_lock(lock);
    }
    else
    {  
        pthread_mutex_t *result_lock = new pthread_mutex_t;
        pthread_mutex_init(result_lock, 0);
        
        result_locks.insert(pair<ResultId, ResultLockInfo*> (result_id, \
                        new ResultLockInfo(result_lock, string(worker_id) )));
        pthread_mutex_unlock(&result_locks_write_lock);
        pthread_mutex_lock(result_lock);
    }
}

void HdividerWatcher::unlockResult(ResultId result_id, string worker_id)
{
    pthread_mutex_lock(&result_locks_write_lock);
  /*  char bf[25];
    sprintf(bf, "%d", (int)result_id);
    LOG("HdividerWatcher::unlockResult "+worker_id+" "+bf);*/
    tr1::unordered_map<ResultId, ResultLockInfo*>::iterator it = result_locks.find(result_id);
    
    if (it!=result_locks.end())
    {
        pthread_mutex_unlock(it->second->lock);
    }
    pthread_mutex_unlock(&result_locks_write_lock);
}