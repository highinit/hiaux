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

//#include "meroving/meroving.h"
#include "../../../hrpc/hcomm/include/hcomm.h"
#include "../../../hrpc/hcomm/include/hcomms.h"

#include "../hdivider_watcher_skel.h"
#include "../hdivider_mongo_accessors.h"
#include "../hdivider_state_accessor_cache.h"
#include "../hdivider_input_accessor_cache.h"

//#include "db_mocks.h"

#include "../../hdivider_core/tests/hdivider_test.h"


using namespace std;

class HdividerTestEnv
{
    map<InputId, int> *input_data;
    map<int, int> *result;
    int first_summ;
    int first_elem_summ;
    int NSPORT;
    hcomm_srv_t *ns;
    hcomm_t *comm_sharing;
    
    vector<HdividerWatcherStub*> watcher_stubs;
    
    void performMultipleWorkersTestWithRpc()
    {
        int nthreads = watcher_stubs.size();
        vector<pthread_t> ths;
        
        for (int i = 0; i<nthreads; i++)
        {
            pthread_t th;
            char bf[255];
            strcpy(bf, "");
            sprintf(bf, "%d", i);
            bf[254] = '\0';
            string worker_id("worker" + string(bf));
            
            worker_args2 *args1 = new worker_args2(watcher_stubs[i], input_data, result, worker_id, 0);
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
        
        TS_ASSERT(first_summ == summ); 
        TS_ASSERT((*result)[9] == first_elem_summ);
    }
    
public:
    
    map<InputId, int> *getInputData()
    {
        return input_data;
    }
    
    void prepareControlResultAndDistributedEnv(int NSPORT, int SPORT, int EPORT, string dbip, int dbport, string dbname, string dbuser, string dbpass, int TEST_SIZE)
    {
        this->NSPORT = NSPORT;
        ns = new hcomm_srv_t(new Hlogger(dbip, dbport, dbname, "logs", "ns", dbuser, dbpass));
        ns->start_server(NSPORT, SPORT, EPORT);
        sleep(1);
                
        comm_sharing = new hcomm_t("127.0.0.1", NSPORT, "watcher_process", new Hlogger(dbip, 27017, dbname, "logs", "ns", dbuser, dbpass));
        comm_sharing->connect();
        
        input_data = new map<InputId, int>;
        result = new map<int, int>;
        
        first_summ = 0;
        first_elem_summ = 0;
        for (int i = 0; i<TEST_SIZE; i++)
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
    }
    
    void performTest(HdividerWatcher* watcher, int nthreads)
    {
        comm_sharing->share_obj<HdividerWatcher, HdividerWatcherSkel>(watcher, "watcher");
        comm_sharing->start_server();
                
        sleep(1);
        
        for (int i = 0; i<nthreads; i++)
        {
            char bf[255];
            strcpy(bf, "");
            sprintf(bf, "%d", i);
            bf[254] = '\0';
            string worker_id("worker" + string(bf));
            
            hcomm_t *comm_client = new hcomm_t("127.0.0.1", NSPORT, worker_id, new Hlogger("127.0.0.1", 27017, "highinit_test", "logs", "ns", "dbuser", "dbuser"));
            comm_client->connect();
            
            watcher_stubs.push_back(new HdividerWatcherStub(comm_client, "watcher"));
        }
        
        
        performMultipleWorkersTestWithRpc();
        
    }
    
    ~HdividerTestEnv()
    {
        ns->kill_server();
        comm_sharing->kill_server();
        delete ns;
        delete comm_sharing;
        delete result;
        delete input_data;
        watcher_stubs.clear();
    }
    
};

class HdividerDistTests : public CxxTest::TestSuite
{    

public:
    
    void testOneWorker()
    {
        const int NSPORT = 5002;
        const int SPORT = 6100;
        const int EPORT = 6200;
        
        
        const int SIZE = 100;
     // multiply on 2 all inputs and write to result. 1 worker   new Hlogger("127.0.0.1", 27017, "highinit_test", "logs", "ns", "dbuser", "dbuser");

        hcomm_srv_t *ns = new hcomm_srv_t(new Hlogger("127.0.0.1", 27017, "highinit_test", "logs", "ns", "dbuser", "dbuser"));
        ns->start_server(NSPORT, SPORT, EPORT);
        sleep(1);
        
        hcomm_t comm_sharing("127.0.0.1", NSPORT, "watcher_process", new Hlogger("127.0.0.1", 27017, "highinit_test", "logs", "ns", "dbuser", "dbuser"));
        comm_sharing.connect();
        
        int first_summ;
        
        map<InputId, int > *input_data = new map<InputId, int > ;
        vector<int> *result = new vector<int>;
        first_summ = 0;
        
        HdividerWatcher* watcher = new HdividerWatcher(new HdividerTestInputIdIt (input_data), \
                new HdividerTestStateAccessor());
        
        comm_sharing.share_obj<HdividerWatcher, HdividerWatcherSkel>(watcher, "watcher");
        comm_sharing.start_server();
                
        sleep(1);
        
        hcomm_t comm_client("127.0.0.1", NSPORT, "node_worker", new Hlogger("127.0.0.1", 27017, "highinit_test", "logs", "ns", "dbuser", "dbuser"));
        comm_client.connect();
            
        HdividerWatcherStub *stub = new HdividerWatcherStub(&comm_client ,"watcher");
         

        for (int i = 0; i<SIZE; i++)
        {
                input_data->insert(pair<InputId, int>(i , i));
                first_summ += i;
        }
        
        worker_func((void*) (new worker_args(stub, input_data, result)));
       
        delete watcher;

        comm_sharing.kill_server();
        
        TS_ASSERT(result->size() == SIZE);
        int summ = 0;
        for (int i = 0; i<result->size(); i++)
        {
            summ += result->at(i);
        }
        ns->kill_server();
    }
        
    void testConcurrentWriteResult()
    {       
        const int NSPORT = 6002;
        const int SPORT = 7100;
        const int EPORT = 7200;
        
        const int SIZE = 100;
        
        HdividerTestEnv *env = new HdividerTestEnv;
        
        try
        {
                env->prepareControlResultAndDistributedEnv(NSPORT, SPORT, EPORT, "127.0.0.1",  27017, "highinit_test", "dbuser", "dbuser", SIZE);
                HdividerWatcher* watcher = new HdividerWatcher(new HdividerTestInputIdIt (env->getInputData()), \
                new HdividerTestStateAccessor());
                env->performTest(watcher, 2);
                
                delete watcher;
        }
        catch (string *s)
        {
            cout << s->c_str();
        }
        
        delete env;       
    }
    
    void testConcurrentWriteStoringHdividerStateInDb()
    {       
        const int NSPORT = 6022;
        const int SPORT = 7120;
        const int EPORT = 7130;
        
        const int SIZE = 50;
        
        HdividerTestEnv *env = new HdividerTestEnv;
        
        try
        {
                env->prepareControlResultAndDistributedEnv(NSPORT, SPORT, EPORT, "127.0.0.1",  27017, "highinit_test", "dbuser", "dbuser", SIZE);
                
                HdividerMongoStateAccessor *state_accessor = new HdividerMongoStateAccessor("127.0.0.1", 27017, "hdivider_dist_test", "testConcurrentWriteResultusingDb", "dbuser", "dbuser"); 
                state_accessor->resetState();
                
                HdividerWatcher* watcher = new HdividerWatcher(new HdividerTestInputIdIt (env->getInputData()), state_accessor);
                env->performTest(watcher, 2);
                
                delete watcher;
        }
        catch (string *s)
        {
            cout << s->c_str();
        }
        
        delete env;       
    }
    
    void testConcurrentWriteResultUsingDbUsingHdividerCache()
    {
        const int NSPORT = 6122;
        const int SPORT = 7220;
        const int EPORT = 7230;
        
        const int SIZE = 50;
        
        HdividerTestEnv *env = new HdividerTestEnv;
        
        try
        {
                env->prepareControlResultAndDistributedEnv(NSPORT, SPORT, EPORT, "127.0.0.1",  27017, "highinit_test", "dbuser", "dbuser", SIZE);
                
                HdividerMongoStateAccessor *mongo_accessor = new HdividerMongoStateAccessor("127.0.0.1", 27017, "hdivider_dist_test", "testConcurrentWriteResultUsingDbUsingHdividerCache", "dbuser", "dbuser"); 
                
                mongo_accessor->resetState();
                
                HdividerStatesCache *states_cache = new HdividerStatesCache(mongo_accessor);
                
                HdividerWatcher* watcher = new HdividerWatcher(new HdividerTestInputIdIt (env->getInputData()), states_cache);
                env->performTest(watcher, 2);

                states_cache->setNoMore();
                states_cache->join();
                delete states_cache;
                delete mongo_accessor;
        }
        catch (string *s)
        {
            cout << s->c_str();
        }
        
        delete env;       
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
    
    void fill_db_testMongoIdCache(string ip, int port, string db_name, string coll_name, string login, string pass, int count)
    {
        mongo conn[1];
        int status = mongo_connect(conn, ip.c_str(), port);

        if( status != MONGO_OK )
        {
          switch ( conn->err )
          {
            case MONGO_CONN_SUCCESS:    {  break; }
            case MONGO_CONN_NO_SOCKET:  { throw new string ("HdividerMongoInputIdIt:: MONGO_CONN_NO_SOCKET" ); return; }
            case MONGO_CONN_FAIL:       { throw new string ("HdividerMongoInputIdIt:: MONGO_CONN_FAIL" ); return; }
            case MONGO_CONN_NOT_MASTER: { throw new string ("HdividerMongoInputIdIt:: MONGO_CONN_NOT_MASTER" ); return; }
          }
        }
        if ( mongo_cmd_authenticate(conn, db_name.c_str(), login.c_str(), pass.c_str()) == MONGO_ERROR )
        {
             throw new string ("HdividerMongoInputIdIt:: MONGO_ERROR error AUTH");
             return;
        }
        
        bson b[1];
        bson_init(b);
        bson_finish(b);
        mongo_remove(conn, (db_name+"."+coll_name).c_str(), b, NULL);
        bson_destroy(b);
        
        for (int i = 0; i<count; i++)
        {
            bson b[1];
            bson_init(b);
            bson_append_long(b, "id", i+(i%5)*2);
            bson_finish(b);
            mongo_insert( conn, (db_name+"."+coll_name).c_str(), b, NULL );
        }
        
        mongo_destroy(conn);
    }
    
    void testMongoIdCache()
    {
        const int SIZE = 10000;
        string ip = "127.0.0.1";
        int port = 27017;
        string db_name = "hdivider_dist_test";
        string coll_name  = "testMongoIdCache_input";      
        string login = "dbuser";
        string pass = "dbuser";
     
        fill_db_testMongoIdCache(ip, port, db_name, coll_name, login, pass, SIZE);
        sleep(2);
        HdividerMongoInputIdIt *input_it = new HdividerMongoInputIdIt(ip, port, db_name, coll_name, login, pass);
        HdividerInputIdCache *id_cache = new HdividerInputIdCache(ip, port, db_name, coll_name, login, pass, 1000);
        
       // input_it->setFirst();
       // id_cache->setFirst();
        
        int same = 0;
        
        for (int i = 0; i<SIZE; i++)
        {
            //cout << input_it->value() << " " << id_cache->value() << " ";
            if (input_it->value() != id_cache->value())
            {
                //cout << "NOTSAME\n";
            }
            else 
            {
                //cout << "SAME\n";
                same++;
            }
            input_it->getNext();
            id_cache->getNext();
        }
        //cout << "SAME: " << same << endl;
        TS_ASSERT(same == SIZE);
        
    }
    
    void testMongoStateAccessor()
    {
        const int SIZE = 100;
        string ip = "127.0.0.1";
        int port = 27017;
        string db_name = "hdivider_dist_test";
        string job_name  = "test";      
        string login = "dbuser";
        string pass = "dbuser";
        
        vector<InputState*> local_states;
        tr1::unordered_map<int64_t, InputState*> *local_states_hashed = new tr1::unordered_map<int64_t, InputState*>;
        
        try
        {
            for (int i = 0; i<SIZE; i++)
            {
                char bf[255];
                sprintf(bf, "worker_%d", i);
                if (i%2==0)
                        local_states.push_back(new InputState(i, bf, 0));
                else
                        local_states.push_back(new InputState(i, bf, 1));
            }
            
            HdividerMongoStateAccessor *states = new HdividerMongoStateAccessor(ip, port, db_name, job_name, login, pass); 
            states->resetState();
            
            for (int i = 0; i<SIZE; i++)
            {
                states->saveState(local_states[i]);
            }
            
            for (int i = 0; i<SIZE; i++)
            {
                InputState *state = states->getState(i);
                
                TS_ASSERT(state->id == local_states[i]->id);
                TS_ASSERT(state->locked_by == local_states[i]->locked_by);
                TS_ASSERT(state->handled == local_states[i]->handled);
                local_states_hashed->insert( pair<int64_t, InputState*>(state->id, state) );
                //delete state;
            }
            
            tr1::unordered_map<int64_t, InputState*>* states_hashed_got = states->getAllStates();
            
            tr1::unordered_map<int64_t, InputState*>::iterator a_it = states_hashed_got->begin();
            tr1::unordered_map<int64_t, InputState*>::iterator b_it = local_states_hashed->begin();
            
            while (b_it!=states_hashed_got->end())
            {
                TS_ASSERT (a_it->first==b_it->first);
                TS_ASSERT (a_it->second->handled==b_it->second->handled);
                TS_ASSERT (a_it->second->locked_by==b_it->second->locked_by);
                a_it++;
                b_it++;      
            }
            
            states->resetState();
            delete states;
            delete states_hashed_got;
            delete local_states_hashed;
            
        }
        catch (string  *s)
        {
            cout << *s;
        }
    }

};
    

#endif	/* HDIVIDER_DIST_TESTS_H */

