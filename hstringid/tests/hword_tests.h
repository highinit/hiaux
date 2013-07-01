/* 
 * File:   hword_tests.h
 * Author: phrk
 *
 * Created on May 7, 2013, 3:11 PM
 */

#ifndef HWORD_TESTS_H
#define	HWORD_TESTS_H


#include <cxxtest/TestSuite.h>

#include "../master/hword_master.h"
#include "../node/hword_node.h"

#include "../../hrpc/hcomm/include/hcomm.h"
#include "../hword_master_skel.h"
#include "../hword_db_accessor.h"



class HwordTests : public CxxTest::TestSuite
{    
    tr1::unordered_map<string, int64_t> control_result;
public:
    
    void doSimpleCalls(HwordNode *node, HwordNode *node2)
    {
        for (int64_t i = 0; i<10; i++)
        {
            char bf[255];
            sprintf(bf, "%lli", i);
            string word(bf);
            int64_t id = node->getId(word);
            //cout << "setID: " << id << endl;
            control_result[word] = id;
        }
        
        for (int64_t i = 5; i<10; i++)
        {
            char bf[255];
            sprintf(bf, "%lli", i);
            string word(bf);
            int64_t id = node->getId(word);
            //cout << "ID: " << id << endl;
            //cout << "cID: " << control_result[word] << endl;
            TS_ASSERT(control_result[word] == id); 
        }
        
        for (int64_t i = 0; i<10; i++)
        {
            char bf[255];
            sprintf(bf, "%lli", i);         
            string word(bf);
            int64_t id = node2->getId(word);
            TS_ASSERT(control_result[word] == id); 
        }
    }
    
    void testSimple()
    {
        HwordMaster *master = new HwordMaster(new HwordDbInteractorStub);
        HwordNode *node = new HwordNode(new HwordMasterIfsSimpleCaller(master), new HwordDbInteractorStub);
        
        HwordNode *node2 = new HwordNode(new HwordMasterIfsSimpleCaller(master), new HwordDbInteractorStub);
        
        control_result.clear();
        
        doSimpleCalls(node, node2);
        
        // cache hits
        TS_ASSERT(node->getStat().first==5);
        // requests = hits + misses
        TS_ASSERT(node->getStat().second==15);
        
        TS_ASSERT(master->getStat().first==10);
        TS_ASSERT(master->getStat().second==20);
        
        delete master;
        delete node;
        delete node2;
    }
    
#define NS_PORT 12789
#define BPORT 12790
#define EPORT 12800
    
    void testDistributed()
    {
        control_result.clear();
 //       hlog_clear("hcomm.log");
 //       hlog_clear("hcomms.log");
        
        hcomm_srv_t ns( new Hlogger("127.0.0.1", 27017, "highinit_test", "logs", "ns", "dbuser", "dbuser"));
        ns.start_server(NS_PORT, BPORT, EPORT);

        hcomm_t share_comm(string("127.0.0.1"), NS_PORT, string("master_node"),  new Hlogger("127.0.0.1", 27017, "highinit_test", "logs", "ns", "dbuser", "dbuser"));
        
        HwordMaster *master = new HwordMaster(new HwordDbInteractorStub);
        share_comm.connect();
        
        share_comm.share_obj<HwordMaster, HwordMasterSkel>(master, "hword_master");
        share_comm.start_server();

        hcomm_t client_comm(string("127.0.0.1"), NS_PORT, string("client_node"),  new Hlogger("127.0.0.1", 27017, "highinit_test", "logs", "ns", "dbuser", "dbuser"));
        client_comm.connect();
        
        HwordMasterStub *master_stub = new HwordMasterStub(&client_comm, "hword_master");
        
        HwordNode *node = new HwordNode(master_stub, new HwordDbInteractorStub);
        HwordNode *node2 = new HwordNode(master_stub, new HwordDbInteractorStub);
        doSimpleCalls(node, node2);
        // cache hits
        TS_ASSERT(node->getStat().first==5);
        // requests = hits + misses
        TS_ASSERT(node->getStat().second==15);
        
        TS_ASSERT(master->getStat().first==10);
        TS_ASSERT(master->getStat().second==20);
        
        
        ns.kill_server();
        share_comm.kill_server();
        
        delete node;
        delete node2;
        delete master;
        delete master_stub;
    }
    
    void testMongoDbAccessor()
    {
        try
        {
            control_result.clear();
            HwordMongoDbAccessor db("127.0.0.1", 27017, "highinit_test", "HwordTest", "dbuser", "dbuser");
            db.clearAll();

            for (int64_t i = 0; i<10; i++)
            {
                char bf[255];
                sprintf(bf, "%lli", i);
                string word(bf);
                db.savePair(word, i);
                control_result[word] = i;
            }

            tr1::unordered_map<string, int64_t> *db_content = db.getIds();

            tr1::unordered_map<string, int64_t>::iterator it = control_result.begin();
            while (it!=control_result.end())
            {
                TS_ASSERT(it->second == (*db_content)[it->first]);
                it++;
            }    
            db.clearAll();
        }
        catch (string *e)
        {
            cout << *e;
        }
  
    }
    
#define NS_PORT 13789
#define BPORT 13790
#define EPORT 13800
    
    void testDistributedWithDb()
    {
        control_result.clear();

        hcomm_srv_t ns(new Hlogger("127.0.0.1", 27017, "highinit_test", "logs", "ns", "dbuser", "dbuser"));
        ns.start_server(NS_PORT, BPORT, EPORT);

        hcomm_t share_comm(string("127.0.0.1"), NS_PORT, string("master_node"), new Hlogger("127.0.0.1", 27017, "highinit_test", "logs", "ns", "dbuser", "dbuser"));
        
        HwordMongoDbAccessor *db = new HwordMongoDbAccessor("127.0.0.1", 27017, "highinit_test", "HwordTest", "dbuser", "dbuser");
        db->clearAll();
        
        HwordMaster *master = new HwordMaster(db);
        share_comm.connect();
        
        share_comm.share_obj<HwordMaster, HwordMasterSkel>(master, "hword_master");
        share_comm.start_server();

        hcomm_t client_comm(string("127.0.0.1"), NS_PORT, string("client_node"), new Hlogger("127.0.0.1", 27017, "highinit_test", "logs", "ns", "dbuser", "dbuser"));
        client_comm.connect();
        
        HwordMasterStub *master_stub = new HwordMasterStub(&client_comm, "hword_master");
        
        HwordNode *node = new HwordNode(master_stub, new HwordMongoDbAccessor("127.0.0.1", 27017, "highinit_test", "HwordTest", "dbuser", "dbuser"));
        HwordNode *node2 = new HwordNode(master_stub, new HwordMongoDbAccessor("127.0.0.1", 27017, "highinit_test", "HwordTest", "dbuser", "dbuser"));
        doSimpleCalls(node, node2);
        
        // cache hits
        TS_ASSERT(node->getStat().first==5);
        // requests = hits + misses
        TS_ASSERT(node->getStat().second==15);
        
        TS_ASSERT(master->getStat().first==10);
        TS_ASSERT(master->getStat().second==20);
        
        doSimpleCalls(node, node2);
        
        ns.kill_server();
        share_comm.kill_server();
        
        // cache hits
        TS_ASSERT(node->getStat().first==20);
        // requests = hits + misses
        TS_ASSERT(node->getStat().second==30);
        
        TS_ASSERT(master->getStat().first==10);
        TS_ASSERT(master->getStat().second==20);
        

        delete node;
        delete node2;
        delete master;
        delete master_stub;
    }
    
};


#endif	/* HWORD_TESTS_H */

