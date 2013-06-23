/* 
 * File:   hdivider_test.h
 * Author: phrk
 *
 * Created on April 20, 2013, 11:59 PM
 */

#ifndef HDIVIDER_TEST_H
#define	HDIVIDER_TEST_H

#define HDIVIDER_TEST

#include <map>
#include <iostream>

#include "../include/hdivider_watcher.h"
#include "../include/hdivider_worker.h"

#include <cxxtest/TestSuite.h>

#include "pthread.h"

#define InputId int64_t

class HdividerTestInputIdIt : public HdividerInputIdIt 
{
    map<int64_t, int > *input_data;
    map<int64_t, int >::iterator it;
    
    
public:
    
    HdividerTestInputIdIt(map<InputId, int > *input_data)
    {
        //cout << "HdividerTestInputIdIt::HdividerTestInputIdIt" << endl;

        this->input_data = input_data;
        setFirst();
    }
    
    virtual int size()
    {
        return input_data->size();
    }
    
    virtual void setFirst()
    {
        //cout << "HdividerTestInputIdIt::setFirst" << endl;
        it = input_data->begin();
    }
    
    virtual void getNext()
    {        
        //cout << "HdividerTestInputIdIt::getNext" << endl;
        it++;
    }
    
    virtual InputId value()
    {
        //cout << "HdividerTestInputIdIt::value" << endl;
        return it->first;
    }
    
    virtual bool end()
    {
        //cout << "HdividerTestInputIdIt::end" << endl;
        return it==input_data->end();
    }
};

class HdividerTestStateAccessor : public HdividerStateAccessor
{
    map<InputId, InputState*> *states;
    pthread_mutex_t mutex;
public:
   
    HdividerTestStateAccessor()
    {
        states = new map<InputId, InputState*>;
        pthread_mutex_init(&mutex, 0);
    }
    
    ~HdividerTestStateAccessor()
    {
        map<InputId, InputState*>::iterator it = states->begin();
        while (it!=states->end())
        {
            delete it->second;
            states->erase(it);
        }
        delete states;
                
    }
    
    virtual InputState *getState(InputId input_id)
    {
       // pthread_mutex_lock(&mutex);
        map<InputId, InputState*>::iterator it = states->find(input_id);
        
        if (it != states->end())
        {
           // pthread_mutex_unlock(&mutex);
            return new InputState(*it->second);
        }
        else
        {
            InputState *state = new InputState(input_id, "", 0);
            states->insert(pair<InputId, InputState*>(input_id, state));
            //pthread_mutex_unlock(&mutex);
            return new InputState(*state);
        }
    }
    
    virtual void saveState(const InputState *state)
    {
        //pthread_mutex_lock(&mutex);
        map<InputId, InputState*>::iterator it = states->find(state->id);
        
        if (it != states->end())
        {
            delete it->second;
            it->second = new InputState(*state);
            //*(it->second) = *state;
        }
        else
        {
            states->insert(pair<InputId, InputState*>(state->id, new InputState(*state)));
        }
        //pthread_mutex_unlock(&mutex);
    }
    
    virtual void resetState()
    {
        //pthread_mutex_lock(&mutex);
        states->clear();
       // pthread_mutex_unlock(&mutex);
    }
};


class HdividerTestWorker : public HdividerWorker
{
    HdividerWatcher *watcher;
public:
    
    
    HdividerTestWorker(HdividerWatcher *watcher)
    {
        this->watcher = watcher;
    }
    
    virtual int isFinished()  
    {
        return watcher->isFinished();
    }
    
    virtual vector<int64_t> getInput(int count, string worker_id)
    {
        return watcher->getInput(count, worker_id);
    }
    
    virtual void setHandled(int64_t input_id)
    {
        watcher->setHandled(input_id);
    }
    
    int isHandled(int64_t input_id)
    {
        return watcher->isHandled(input_id);
    }
    
    virtual void lockResult(int64_t result_id, string worker_id)
    {
        watcher->lockResult(result_id, worker_id);
    }
    
    virtual void unlockResult(int64_t result_id, string worker_id)
    {
        watcher->unlockResult(result_id, worker_id);
    }
    
    virtual void fault(string worker_id)
    {
        watcher->handleFault(worker_id);
    }
};

class worker_args
{
public:
    HdividerWorker *hdivider;
    map<InputId, int > *input_data;
    vector<int> *result;
    
    worker_args(HdividerWorker *hdivider, map<InputId, int > *input_data, vector<int> *result)
    {
        this->hdivider = hdivider;
        this->input_data = input_data;
        this->result = result;
    }
};

class worker_args2
{
public:
    HdividerWorker *hdivider;
    map<InputId, int> *input_data;
    map<int, int> *result;
    string worker_id;
    bool fault;
    
    worker_args2(HdividerWorker *hdivider, \
        map<InputId, int > *input_data,\
         map<int, int> *result,\
         string worker_id, \
        bool fault)
    {
        this->hdivider = hdivider;
        this->input_data = input_data;
        this->result = result;
        this->worker_id = worker_id;
        this->fault = fault;
    }
};

void worker_func(void *a);
void *worker_func2(void *a);
void *worker_func3(void *a);

class HdividerTests : public CxxTest::TestSuite
{    
public:
    
    static pthread_mutex_t result_lock;
    
    HdividerTests()
    {
             pthread_mutex_init(&result_lock, 0);
    }
    
    void testOneWorker();
    
    void testConcurrentReadInput();
    
    void testConcurrentWriteResult();
    
    void testOneFault();
};

#endif	/* HDIVIDER_TEST_H */

