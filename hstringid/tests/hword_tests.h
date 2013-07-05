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
#include "../node/hword_node_morphology.h"



#include "../../hrpc/hcomm/include/hcomm.h"
#include "../hword_master_skel.h"
#include "../hword_db_accessor.h"

#include "../english_stemmer.h"

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
        HwordMaster *master = new HwordMaster(new HwordDbInteractorStub, new HwordDbInteractorStub, CACHE_ENABLED);
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
        
        hcomm_srv_t ns( new Hlogger("127.0.0.1", 27017, "hstringid_test", "logs", "ns", "dbuser", "dbuser"));
        ns.start_server(NS_PORT, BPORT, EPORT);

        hcomm_t share_comm(string("127.0.0.1"), NS_PORT, string("master_node"),  new Hlogger("127.0.0.1", 27017, "hstringid_test", "logs", "ns", "dbuser", "dbuser"));
        
        HwordMaster *master = new HwordMaster(new HwordDbInteractorStub, new HwordDbInteractorStub, CACHE_ENABLED);
        share_comm.connect();
        
        share_comm.share_obj<HwordMaster, HwordMasterSkel>(master, "hword_master");
        share_comm.start_server();

        hcomm_t client_comm(string("127.0.0.1"), NS_PORT, string("client_node"),  new Hlogger("127.0.0.1", 27017, "hstringid_test", "logs", "ns", "dbuser", "dbuser"));
        client_comm.connect();
        
        HwordMasterStub *master_stub = new HwordMasterStub(&client_comm, "hword_master");
        
        HwordNode *node = new HwordNode(master_stub, CACHE_ENABLED);//new HwordDbInteractorStub);
        HwordNode *node2 = new HwordNode(master_stub, CACHE_ENABLED);//new HwordDbInteractorStub);
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
            HwordMongoDbAccessor db("127.0.0.1", 27017, "hstringid_test", "testMongoDbAccessor", "dbuser", "dbuser");
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

        hcomm_srv_t ns(new Hlogger("127.0.0.1", 27017, "hstringid_test", "logs", "ns", "dbuser", "dbuser"));
        ns.start_server(NS_PORT, BPORT, EPORT);

        hcomm_t share_comm(string("127.0.0.1"), NS_PORT, string("master_node"), new Hlogger("127.0.0.1", 27017, "hstringid_test", "logs", "ns", "dbuser", "dbuser"));
        
        HwordMongoDbAccessor *db = new HwordMongoDbAccessor("127.0.0.1", 27017, "hstringid_test", "testDistributedWithDb", "dbuser", "dbuser");
        db->clearAll();
        
        HwordMongoDbAccessor *db_write = new HwordMongoDbAccessor("127.0.0.1", 27017, "hstringid_test", "testDistributedWithDb", "dbuser", "dbuser");
        
        HwordMaster *master = new HwordMaster(db, db_write, CACHE_ENABLED);
        share_comm.connect();
        
        share_comm.share_obj<HwordMaster, HwordMasterSkel>(master, "hword_master");
        share_comm.start_server();

        hcomm_t client_comm(string("127.0.0.1"), NS_PORT, string("client_node"), new Hlogger("127.0.0.1", 27017, "hstringid_test", "logs", "ns", "dbuser", "dbuser"));
        client_comm.connect();
        
        HwordMasterStub *master_stub = new HwordMasterStub(&client_comm, "hword_master");
        
        HwordNode *node = new HwordNode(master_stub, CACHE_ENABLED);//new HwordMongoDbAccessor("127.0.0.1", 27017, "hstringid_test", "testDistributedWithDb", "dbuser", "dbuser"));
        HwordNode *node2 = new HwordNode(master_stub, CACHE_ENABLED);//new HwordMongoDbAccessor("127.0.0.1", 27017, "hstringid_test", "testDistributedWithDb", "dbuser", "dbuser"));
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
    
    
       
#define NS_PORT 14789
#define BPORT 14790
#define EPORT 14800
    
    
    void testServerCacheDisabledOneWorkerWithCacheEnabled()
    {
        control_result.clear();

        hcomm_srv_t ns(new Hlogger("127.0.0.1", 27017, "hstringid_test", "logs", "ns", "dbuser", "dbuser"));
        ns.start_server(NS_PORT, BPORT, EPORT);

        hcomm_t share_comm(string("127.0.0.1"), NS_PORT, string("master_node"), new Hlogger("127.0.0.1", 27017, "hstringid_test", "logs", "ns", "dbuser", "dbuser"));
        
        HwordMongoDbAccessor *db = new HwordMongoDbAccessor("127.0.0.1", 27017, "hstringid_test", "testServerCacheDisabled", "dbuser", "dbuser");
        db->clearAll();
        
        HwordMongoDbAccessor *db_write = new HwordMongoDbAccessor("127.0.0.1", 27017, "hstringid_test", "testServerCacheDisabled", "dbuser", "dbuser");
        
        HwordMaster *master = new HwordMaster(db, db_write, CACHE_DISABLED);
        share_comm.connect();
        
        share_comm.share_obj<HwordMaster, HwordMasterSkel>(master, "hword_master");
        share_comm.start_server();

        hcomm_t client_comm(string("127.0.0.1"), NS_PORT, string("client_node"), new Hlogger("127.0.0.1", 27017, "hstringid_test", "logs", "ns", "dbuser", "dbuser"));
        client_comm.connect();
        
        HwordMasterStub *master_stub = new HwordMasterStub(&client_comm, "hword_master");
        
        HwordNode *node = new HwordNode(master_stub, CACHE_ENABLED);//new HwordMongoDbAccessor("127.0.0.1", 27017, "hstringid_test", "testServerCacheDisabled", "dbuser", "dbuser"));
        //HwordNode *node2 = new HwordNode(master_stub, CACHE_ENABLED);//new HwordMongoDbAccessor("127.0.0.1", 27017, "hstringid_test", "testServerCacheDisabled", "dbuser", "dbuser"));
        doSimpleCalls(node, node);
        
      //  doSimpleCalls(node, node2);
        
        ns.kill_server();
        share_comm.kill_server();

        delete node;
//        delete node2;
        delete master;
        delete master_stub;
    }
        
#define NS_PORT 15789
#define BPORT 15790
#define EPORT 15800
    
    void testLocalCacheDisabled()
    {
        control_result.clear();

        hcomm_srv_t ns(new Hlogger("127.0.0.1", 27017, "hstringid_test", "logs", "ns", "dbuser", "dbuser"));
        ns.start_server(NS_PORT, BPORT, EPORT);

        hcomm_t share_comm(string("127.0.0.1"), NS_PORT, string("master_node"), new Hlogger("127.0.0.1", 27017, "hstringid_test", "logs", "ns", "dbuser", "dbuser"));
        
        HwordMongoDbAccessor *db = new HwordMongoDbAccessor("127.0.0.1", 27017, "hstringid_test", "testLocalCacheDisabled", "dbuser", "dbuser");
        db->clearAll();
        
        HwordMongoDbAccessor *db_write = new HwordMongoDbAccessor("127.0.0.1", 27017, "hstringid_test", "testLocalCacheDisabled", "dbuser", "dbuser");
        
        HwordMaster *master = new HwordMaster(db, db_write, CACHE_ENABLED);
        share_comm.connect();
        
        share_comm.share_obj<HwordMaster, HwordMasterSkel>(master, "hword_master");
        share_comm.start_server();

        hcomm_t client_comm(string("127.0.0.1"), NS_PORT, string("client_node"), new Hlogger("127.0.0.1", 27017, "hstringid_test", "logs", "ns", "dbuser", "dbuser"));
        client_comm.connect();
        
        HwordMasterStub *master_stub = new HwordMasterStub(&client_comm, "hword_master");
        
        HwordNode *node = new HwordNode(master_stub, CACHE_DISABLED);// new HwordMongoDbAccessor("127.0.0.1", 27017, "hstringid_test", "testLocalCacheDisabled", "dbuser", "dbuser"));
        HwordNode *node2 = new HwordNode(master_stub, CACHE_DISABLED);// new HwordMongoDbAccessor("127.0.0.1", 27017, "hstringid_test", "testLocalCacheDisabled", "dbuser", "dbuser"));
        doSimpleCalls(node, node2);
        
      //  doSimpleCalls(node, node2);
        
        ns.kill_server();
        share_comm.kill_server();

        delete node;
        delete node2;
        delete master;
        delete master_stub;
    }
    
    void testStemmer()
    {
        HenglishStemmer stemm;
        TS_ASSERT("ti" == stemm.stemWord("ties"));
    }
    
#define NS_PORT 16789
#define BPORT 16790
#define EPORT 16800
    
    void testDistributedWithStemmer()
    {
        control_result.clear();

        hcomm_srv_t ns(new Hlogger("127.0.0.1", 27017, "hstringid_test", "logs", "ns", "dbuser", "dbuser"));
        ns.start_server(NS_PORT, BPORT, EPORT);

        hcomm_t share_comm(string("127.0.0.1"), NS_PORT, string("master_node"), new Hlogger("127.0.0.1", 27017, "hstringid_test", "logs", "ns", "dbuser", "dbuser"));
        
        HwordMongoDbAccessor *db = new HwordMongoDbAccessor("127.0.0.1", 27017, "hstringid_test", "testDistributedWithStemmer", "dbuser", "dbuser");
        db->clearAll();
        
        HwordMongoDbAccessor *db_write = new HwordMongoDbAccessor("127.0.0.1", 27017, "hstringid_test", "testDistributedWithStemmer", "dbuser", "dbuser");
        
        HwordMaster *master = new HwordMaster(db, db_write, CACHE_ENABLED);
        share_comm.connect();
        
        share_comm.share_obj<HwordMaster, HwordMasterSkel>(master, "hword_master");
        share_comm.start_server();

        hcomm_t client_comm(string("127.0.0.1"), NS_PORT, string("client_node"), new Hlogger("127.0.0.1", 27017, "hstringid_test", "logs", "ns", "dbuser", "dbuser"));
        client_comm.connect();
        
        HwordMasterStub *master_stub = new HwordMasterStub(&client_comm, "hword_master");
        
        HenglishStemmer *stemmer = new HenglishStemmer;
        
        HwordNodeMorphology *node = new HwordNodeMorphology(master_stub, stemmer);// new HwordMongoDbAccessor("127.0.0.1", 27017, "hstringid_test", "testLocalCacheDisabled", "dbuser", "dbuser"));
        HwordNodeMorphology *node2 = new HwordNodeMorphology(master_stub, stemmer);// new HwordMongoDbAccessor("127.0.0.1", 27017, "hstringid_test", "testLocalCacheDisabled", "dbuser", "dbuser"));
        doSimpleCalls(node, node2);
        
      //  doSimpleCalls(node, node2);
        
        ns.kill_server();
        share_comm.kill_server();

        delete node;
        delete node2;
        delete master;
        delete master_stub;
    }
    
};


#endif	/* HWORD_TESTS_H */

