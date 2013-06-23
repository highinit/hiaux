/* 
 * File:   hdivider_watcher.h
 * Author: phrk
 *
 * Created on April 20, 2013, 12:01 AM
 */

#ifndef HDIVIDER_WATCHER_H
#define	HDIVIDER_WATCHER_H
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "pthread.h"

#include <tr1/unordered_map>

//#include "meroving/meroving.h"
#include "../../../hrpc/hcomm/include/common.h"
#include "../../../hrpc/hcomm/include/hcomm.h"
#include "../../../hrpc/hcomm/include/hcomms.h"
#include "../../../hrpc/hcomm/include/objrepr.h"
using namespace std;

#define InputId int64_t
//typedef string InputId;

#define ResultId int64_t



class InputState
{
public:
    InputId id;
    string locked_by;
    bool handled;
    
    InputState();
    InputState(const InputState &state);
    InputState(InputId id, string locked_by, bool handled);
};

class HdividerInputIdIt
{
public:
    virtual int size() = 0;
    virtual void setFirst() = 0;
    virtual void getNext() = 0;
    virtual InputId value() = 0;
    virtual bool end() = 0;
};

class HdividerStateAccessor
{
public:
   // virtual bool notLocked(InputId input_id) = 0;
    virtual InputState *getState(InputId input_id) = 0;
    virtual void saveState(const InputState *state) = 0;
    virtual void resetState() = 0;
};

class ResultLockInfo
{
public:
    pthread_mutex_t *lock;
    string worker_id;
    
    ResultLockInfo(pthread_mutex_t* lock, string worker_id);
    ~ResultLockInfo();
};

class HdividerWatcher
{
    HdividerInputIdIt *input_id_it; 
    HdividerStateAccessor *state_accessor;
    bool finished;
    int nhandled;

    pthread_mutex_t put_mutex;
    pthread_mutex_t get_mutex;
    
    tr1::unordered_map<ResultId, ResultLockInfo*> result_locks;
    pthread_mutex_t result_locks_write_lock;
    
public:
    
    HdividerWatcher();
    
    HdividerWatcher(HdividerInputIdIt *input_id_it, HdividerStateAccessor *state_accessor);
    ~HdividerWatcher();
    void start(int port);
    void resume(int port);
    void handleFault(string worker_id);
    
    bool isFinished();
    
    vector<int64_t> getInput(int count, string worker_id);
    
    void setHandled(InputId input_id);
    int isHandled(InputId input_id);
    void lockResult(ResultId result_id, string worker_id);
    void unlockResult(ResultId result_id, string worker_id);
    
};

#endif	/* HDIVIDER_WATCHER_H */

