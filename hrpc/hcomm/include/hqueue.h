#pragma once
#include "common.h"

class HQUEUE_EX
{
public:
    
    char *str;
    
    HQUEUE_EX(char *s)
    {
        str = s;
    }
    
};

template<class type>
class hqueue_t
{
    class node_t
    {
    public:
        node_t *next;
        type item;
        
        node_t(type item, node_t *next)
        {
            this->item = item;
            this->next = next;
        }
        
        ~node_t()
        {
            if (next!=NULL) delete next;
        }
        
        
        
    };
    
    node_t *head;
    node_t *tail;
    
    pthread_mutex_t lock_push;
    pthread_mutex_t lock_pop;
    pthread_mutex_t lock_empty_var;
    pthread_mutex_t lock_empty;
    
    bool empty;
    int nsources;
    
    pthread_mutex_t lock_sources;
    
public:
    
    hqueue_t()
    {
        pthread_mutex_init(&lock_push, NULL);
        pthread_mutex_init(&lock_pop, NULL);
        pthread_mutex_init(&lock_empty, NULL);
        pthread_mutex_init(&lock_empty_var, NULL);
        pthread_mutex_init(&lock_sources, NULL);
        
        head = tail = new node_t(NULL, NULL);
        empty = 1;
        nsources = 0;
    }
    
    ~hqueue_t()
    {
        pthread_mutex_destroy(&lock_push);
        pthread_mutex_destroy(&lock_pop);
        pthread_mutex_destroy(&lock_empty);
        pthread_mutex_destroy(&lock_empty_var);
        pthread_mutex_destroy(&lock_sources);
        delete head;
    }
    
    void push(type item)
    {
        node_t *node = new node_t(item, NULL);
        pthread_mutex_lock(&lock_push);
        tail->next = node;
        tail = node;
        
        pthread_mutex_lock(&lock_empty_var);
        if (empty)
        {
            empty = 0;
            pthread_mutex_unlock(&lock_empty);
        }
        pthread_mutex_unlock(&lock_empty_var);
        pthread_mutex_unlock(&lock_push);
    }
    
    type pop()
    {
        type item = NULL;
        pthread_mutex_lock(&lock_pop);
        // empty
        if (head->next==NULL)
        {
            pthread_mutex_lock(&lock_empty_var);
            empty = 1;
            pthread_mutex_unlock(&lock_empty_var);
            
            pthread_mutex_lock(&lock_sources);
            // empty && src == 0
            if (nsources==0)
            {
                pthread_mutex_unlock(&lock_sources);
                pthread_mutex_unlock(&lock_pop);
                pthread_mutex_unlock(&lock_empty);
                throw "no more";
            }
            pthread_mutex_unlock(&lock_sources);
            // empty && src > 0
            pthread_mutex_lock(&lock_empty);
            pthread_mutex_lock(&lock_empty);
            pthread_mutex_unlock(&lock_empty);
        }
        
        // if dec_src called
        pthread_mutex_lock(&lock_sources);
        pthread_mutex_lock(&lock_empty_var);
        if (empty && nsources==0)
        {
            pthread_mutex_unlock(&lock_empty_var);
            pthread_mutex_unlock(&lock_sources);
            pthread_mutex_unlock(&lock_pop);
            pthread_mutex_unlock(&lock_empty);
            throw "no more";
        }
        pthread_mutex_unlock(&lock_empty_var);
        pthread_mutex_unlock(&lock_sources);
        
        item = head->next->item;
        
        node_t *next = head->next;
        head->next = NULL;
        delete head;
        head = next;
        
        pthread_mutex_unlock(&lock_pop);
        return item;
    }
    
    void set_nsources(int s)
    {
        pthread_mutex_lock(&lock_sources);
        nsources = s;
        pthread_mutex_unlock(&lock_sources);
    }
    
    void dec_sources()
    {
        pthread_mutex_lock(&lock_sources);
        nsources--;
        if (nsources==0)
        {
            pthread_mutex_unlock(&lock_sources);
            for (int i = 0; i<10; i++)
                pthread_mutex_unlock(&lock_empty);
        }
        else pthread_mutex_unlock(&lock_sources);
    }
    
    
    
    int size()
    {
        throw "A";
    }
};

#include <deque>

template<class T>
class q_t
{
    deque<T> q;
    sem_t sem;
public:
    
    q_t()
    {
        sem_init(&sem, 0, 1);
    }
    
    
    void push(T m)
    {
        sem_wait(&sem);
        
        q.push_back(m);
        sem_post(&sem);
    }
    
    T pop()
    {
        sem_wait(&sem);
        T v;
        if (q.size()!=0)
        {
            v = q.back();
            q.pop_back();
        }
        else v = NULL;
        sem_post(&sem);
        return v;
    }
};