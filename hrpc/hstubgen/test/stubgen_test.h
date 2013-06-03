#include "../src/stubgen.h"
#include "rpc_server_class_skel.h"
#include "rpc_server_class.h"

#define LOG(str) hlog_log(str, "hcomms.log")

class stubgen_test : public CxxTest::TestSuite
{
    public:

	stubgen_t *stubgen;

	stubgen_test()
	{
                //stubgen = new stubgen_t();
                //stubgen->gen("hfss_t", "hfss.h", "skelhfs.h");
                //stubgen->gen("RpcServer", "rpc_server_class.h", "rpc_server_class_skel.h");
	}

        void test1()
        {
            RpcServer *obj = new RpcServer;
            
            #define NSPORT 6001
            #define SPORT 7000
            #define EPORT 7010
            
            try
            {
            
            hlog_clear("hcomm.log");
            hlog_clear("hcomms.log");
            
            hcomm_srv_t ns;
            ns.start_server(NSPORT, SPORT, EPORT);
            
            sleep(2);
            
            hcomm_t comm_sharing("127.0.0.1", NSPORT, "node1");
            comm_sharing.connect();
            
            sleep(5);
            
            comm_sharing.share_obj<RpcServer, RpcServerskel_t>(obj, "RpcServer");
            comm_sharing.start_server();
            
            
            
            sleep(5);
            
            
            // this shit in new thread
            hcomm_t comm_client("127.0.0.1", NSPORT, "node2");
            comm_client.connect();
            
            RpcServerstub_t *stub = new RpcServerstub_t(&comm_client ,"RpcServer");
         
            
            // call
            for (int i =0; i<10000; i++)
            {
                stub->setHandled(4);
                stub->lockResult(5, "aisdahsodihaioshdaiosdhiasd");
                stub->unlockResult(5, "aisdahsodihaioshdaiosdhiasd");
            }
            // call
            
            vector<int> inp;
            for (int i =0; i<1000; i++)
            inp=  stub->getInput(2, "worked_id_1");
            
            TS_ASSERT_EQUALS(inp.size(), 3);
            for (int i = 0; i<inp.size(); i++)
            {
                TS_ASSERT_EQUALS(inp[i], 2+i);
            }
            
            // call
            for (int i =0; i<10000; i++)
            TS_ASSERT_EQUALS(1, stub->isFinished());
            
            stub->close();
            // check this
            ns.kill_server();
            comm_sharing.kill_server();
            delete stub;
            }
            catch (string* s)
            {
                LOG(*s);
            }
            //comm.share_obj()
        }
        
};
/*
void printall(pair<char*, int> *p)
{
	for (int i = 0; i<p->second; i++)
		printf("%c", p->first[i]);
}

TEST_F(stubgen_test, ExtractTypes)
{
    	::testing::FLAGS_gtest_death_test_style = "threadsafe";
	//vecpair_t *f = stubgen->get_type("read_ret_t");
	//printf("!!! %s !!!", f->at(0).second);
	//EXPECT_EQ(0, strcmp(f->at(0).first,"vector<bar>") );
	//EXPECT_EQ(0, strcmp(f->at(0).second,"a ") );

//	EXPECT_EQ(0, strcmp(f->at(1).first,"int") );
//	EXPECT_EQ(0, strcmp(f->at(1).second,"crc ") );
	type_t *type = stubgen->type->at(5);
//	repr_gen::init();
//	printall(repr_gen::gen_all(type));
}
*/