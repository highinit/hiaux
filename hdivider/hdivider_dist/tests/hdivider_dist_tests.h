/* 
 * File:   hdivider_dist_tests.h
 * Author: phrk
 * Tests using meroving rpc
 * Created on April 28, 2013, 10:47 PM
 */

#ifndef HDIVIDER_DIST_TESTS_H
#define	HDIVIDER_DIST_TESTS_H

#include <map>
#include <iostream>

#include <cxxtest/TestSuite.h>

#include "pthread.h"

#include "../../hdivider_core/include/hdivider_watcher.h"
#include "../../hdivider_core/include/hdivider_worker.h"

#include "meroving/meroving.h"

#include "../hdivider_watcher_skel.h"
#include "../hdivider_mongo_accessors.h"

//#include "db_mocks.h"

#include "../../hdivider_core/tests/hdivider_test.h"


using namespace std;


class HdividerDistTests : public CxxTest::TestSuite
{    
public:
    
    #define NSPORT 6002
    #define SPORT 7100
    #define EPORT 7110
    
    void ltestOneWorker()
    {
     // multiply on 2 all inputs and write to result. 1 worker   
        
        hlog_clear("hcomm.log");
        hlog_clear("hcomms.log");
        hlog_clear("HdividerWatcher.log");

        
        hcomm_srv_t ns;
        ns.start_server(NSPORT, SPORT, EPORT);
        
        sleep(2);
            
        hcomm_t comm_sharing("127.0.0.1", NSPORT, "watcher_process");
        comm_sharing.connect();
        
        int first_summ;
        
        map<InputId, int > *input_data = new map<InputId, int > ;
        vector<int> *result = new vector<int>;
        first_summ = 0;
        
        HdividerWatcher* watcher = new HdividerWatcher(new HdividerTestInputIdIt (input_data), \
                new HdividerTestStateAccessor());
        
        comm_sharing.share_obj<HdividerWatcher, HdividerWatcherSkel>(watcher, "watcher");
        comm_sharing.start_server();
                
        sleep(5);
        
        hcomm_t comm_client("127.0.0.1", NSPORT, "node_worker");
        comm_client.connect();
            
        HdividerWatcherStub *stub = new HdividerWatcherStub(&comm_client ,"watcher");
         

        for (int i = 0; i<1000; i++)
        {
                input_data->insert(pair<InputId, int>(i , i));
                first_summ += i;
        }
        
        worker_func((void*) (new worker_args(stub, input_data, result)));
       
        delete watcher;
        ns.kill_server();
        comm_sharing.kill_server();
        
        TS_ASSERT(result->size() == 1000);
        int summ = 0;
        for (int i = 0; i<result->size(); i++)
        {
            summ += result->at(i);
        }
        
    }
     
    void ltestConcurrentWriteResult()
    {
        try
        {
        hlog_clear("hcomm.log");
        hlog_clear("hcomms.log");
        
        hcomm_srv_t *ns = new hcomm_srv_t();
        ns->start_server(NSPORT, SPORT, EPORT);
            
        sleep(2);
            
        hcomm_t *comm_sharing = new hcomm_t("127.0.0.1", NSPORT, "watcher_process");
        comm_sharing->connect();
        
        map<InputId, int > *input_data = new map<InputId, int >;
        map<int, int> *result = new map<int, int>;
        
        int first_summ = 0;
        int first_elem_summ = 0;
        for (int i = 0; i<1000; i++)
        {
                input_data->insert(pair<InputId, int>(i , i));
                
                if (i%2==0)
                {
                    first_summ +=2;
                }
                if (i%3==0)
                {
                    first_summ +=3;
                }
                if (i%5==0)
                {
                    first_summ += 5;
                }
                if (i%7==0)
                {
                    first_summ += 7;
                }
                first_elem_summ += i;
        }
        
        (*result)[2] = (*result)[3] = (*result)[5] = (*result)[7] = (*result)[9] = 0;
        
        HdividerWatcher* watcher = new HdividerWatcher(new HdividerTestInputIdIt (input_data), \
                new HdividerTestStateAccessor());
        
        comm_sharing->share_obj<HdividerWatcher, HdividerWatcherSkel>(watcher, "watcher");
        comm_sharing->start_server();
                
        sleep(5);
                
        int nthreads = 2;
        vector<pthread_t> ths;
        
        
        
        for (int i = 0; i<nthreads; i++)
        {
            pthread_t th;
            char bf[255];
            strcpy(bf, "");
            sprintf(bf, "%d", i);
            bf[254] = '\0';
            string worker_id("worker" + string(bf));
            
            hcomm_t *comm_client = new hcomm_t("127.0.0.1", NSPORT, worker_id);
            comm_client->connect();
            
            HdividerWatcherStub *stub = new HdividerWatcherStub(comm_client, "watcher");
            
            worker_args2 *args1 = new worker_args2(stub, input_data, result, worker_id, 0);
            pthread_create(&th, NULL, worker_func3, (void*)args1);
            ths.push_back(th);
        }
     
        for (int i = 0; i<nthreads; i++)
        {
                pthread_join(ths[i], NULL);
        }
        
         for (int i = 0; i<nthreads; i++)
        {
                pthread_detach(ths[i]);
        }
        
        int summ = 0;
        
        summ += (*result)[2] + (*result)[3] + (*result)[5] +(*result)[7];
        
        delete watcher;
        ns->kill_server();
        delete ns;
        comm_sharing->kill_server();
        delete comm_sharing;
        
        TS_ASSERT(first_summ == summ);
        cout << "first_summ: " << first_summ << endl;
        cout << "summ: " << summ << endl;
        
        // read test
        TS_ASSERT((*result)[9] == first_elem_summ);
        cout << "first_summ: " << first_elem_summ << endl;
        cout << "summ: " << (*result)[9] << endl;
        
        delete result;
        delete input_data;
        }
        catch (string *s)
        {
            LOG(string("EXCEPTION") + *s);
            exit(0);
        }
        
    }
    
    void testConcurrentWriteResultusingDb()
    {
        try
        {
        hlog_clear("hcomm.log");
        hlog_clear("hcomms.log");
        
        hcomm_srv_t *ns = new hcomm_srv_t();
        ns->start_server(NSPORT, SPORT, EPORT);
            
        sleep(2);
            
        hcomm_t *comm_sharing = new hcomm_t("127.0.0.1", NSPORT, "watcher_process");
        comm_sharing->connect();
        
        map<InputId, int > *input_data = new map<InputId, int >;
        map<int, int> *result = new map<int, int>;
        
        int first_summ = 0;
        int first_elem_summ = 0;
        for (int i = 0; i<500; i++)
        {
                input_data->insert(pair<InputId, int>(i , i));
                
                if (i%2==0)
                {
                    first_summ +=2;
                }
                if (i%3==0)
                {
                    first_summ +=3;
                }
                if (i%5==0)
                {
                    first_summ += 5;
                }
                if (i%7==0)
                {
                    first_summ += 7;
                }
                first_elem_summ += i;
        }
        
        (*result)[2] = (*result)[3] = (*result)[5] = (*result)[7] = (*result)[9] = 0;
        
        HdividerMongoStateAccessor *state_accessor = new HdividerMongoStateAccessor("127.0.0.1", 27017, "hdivider_dist_test", "testConcurrentWriteResultusingDb", "dbuser", "dbuser"); 
        
        HdividerWatcher* watcher = new HdividerWatcher(new HdividerTestInputIdIt (input_data), 
                state_accessor);
        
        comm_sharing->share_obj<HdividerWatcher, HdividerWatcherSkel>(watcher, "watcher");
        comm_sharing->start_server();
                
        sleep(5);
                
        int nthreads = 5;
        vector<pthread_t> ths;
        
        
        
        for (int i = 0; i<nthreads; i++)
        {
            pthread_t th;
            char bf[255];
            strcpy(bf, "");
            sprintf(bf, "%d", i);
            bf[254] = '\0';
            string worker_id("worker" + string(bf));
            
            hcomm_t *comm_client = new hcomm_t("127.0.0.1", NSPORT, worker_id);
            comm_client->connect();
            
            HdividerWatcherStub *stub = new HdividerWatcherStub(comm_client, "watcher");
            
            worker_args2 *args1 = new worker_args2(stub, input_data, result, worker_id, 0);
            pthread_create(&th, NULL, worker_func3, (void*)args1);
            ths.push_back(th);
        }
     
        for (int i = 0; i<nthreads; i++)
        {
                pthread_join(ths[i], NULL);
        }
        
         for (int i = 0; i<nthreads; i++)
        {
                pthread_detach(ths[i]);
        }
        
        int summ = 0;
        
        summ += (*result)[2] + (*result)[3] + (*result)[5] +(*result)[7];
        
        delete watcher;
        ns->kill_server();
        delete ns;
        comm_sharing->kill_server();
        delete comm_sharing;
        
        TS_ASSERT(first_summ == summ);
        cout << "first_summ: " << first_summ << endl;
        cout << "summ: " << summ << endl;
        
        // read test
        TS_ASSERT((*result)[9] == first_elem_summ);
        cout << "first_summ: " << first_elem_summ << endl;
        cout << "summ: " << (*result)[9] << endl;
        
        delete result;
        delete input_data;
        }
        catch (string *s)
        {
            LOG(string("EXCEPTION") + *s);
            exit(0);
        }
        
    }
    
    void testMongoIdAccessor()
    {
        string ip = "127.0.0.1";
        int port = 27017;
        string db_name = "hdivider_dist_test";
        string coll_name  = "input";      
        string login = "dbuser";
        string pass = "dbuser";
        
        try
        {  
                HdividerMongoInputIdIt *input_it = new HdividerMongoInputIdIt(ip, port, db_name, coll_name, login, pass);
        
                TS_ASSERT(input_it->size()==2);
        
                int i = 1;
                while (!input_it->end())
                {
                        TS_ASSERT(input_it->value() == i);        
                        input_it->getNext();
                        i++;
                }
                
                input_it->setFirst();
                i = 1;
                while (!input_it->end())
                {
                        TS_ASSERT(input_it->value() == i);        
                        input_it->getNext();
                        i++;
                }
                delete input_it;
                
        }
        catch (string  *s)
        {
            cout << *s;
        }
    }
    
    void testMongoStateAccessor()
    {
        string ip = "127.0.0.1";
        int port = 27017;
        string db_name = "hdivider_dist_test";
        string job_name  = "test";      
        string login = "dbuser";
        string pass = "dbuser";
        
        vector<InputState*> local_states;
        
        try
        {
            for (int i = 0; i<10; i++)
            {
                char bf[255];
                sprintf(bf, "worker_%d", i);
                if (i%2==0)
                        local_states.push_back(new InputState(i, bf, 0));
                else
                        local_states.push_back(new InputState(i, bf, 1));
            }
            
            HdividerMongoStateAccessor *states = new HdividerMongoStateAccessor(ip, port, db_name, job_name, login, pass); 
            
            for (int i = 0; i<10; i++)
            {
                states->saveState(local_states[i]);
            }
            
            for (int i = 0; i<10; i++)
            {
                InputState *state = states->getState(i);
                
                TS_ASSERT(state->id == local_states[i]->id);
                TS_ASSERT(state->locked_by == local_states[i]->locked_by);
                TS_ASSERT(state->handled == local_states[i]->handled);
                
                delete state;
            }
            
            delete states;
        }
        catch (string  *s)
        {
            cout << *s;
        }
    }

};
    

#endif	/* HDIVIDER_DIST_TESTS_H */

