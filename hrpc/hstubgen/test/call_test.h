#pragma once
//#include "../msvs/params_combinations.h"
#include "../../hcomm/include/common.h"
#include "../../hcomm/include/hcomm.h"
#include "../src/stubgen.h"
#include <cxxtest/TestSuite.h>
//#include "gtest/gtest.h"

/*	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(1,1), &wsaData))
		{
		throw "hsock_t::server: Wsastartup fail";;
		return 0;
		}
*/


//#define REGEN


// stubgen_t unit test
class call_test_suite : public CxxTest::TestSuite
{
	public:

	stubgen_t *stubgen;

	virtual void setUp()
	{

	stubgen = new stubgen_t();
//#ifdef REGEN
	//stubgen->gen("remote_class_t", "params_combinations.h", "skelcomb.h");
//#endif
	}
#ifndef REGEN
#include "../test/skelcomb.h"

// start remote object
hcomm_t* remote_obj_start(remote_class_t *obj)
{
	hcomm_t *h = new hcomm_t("127.0.0.1", 5557, "server proc");
	try
	{
		h->connect();
	}
	catch(char *s)
	{
		printf("%s", s);
	}
	h->share_obj<remote_class_t, remote_class_tskel_t>(obj, "remote_obj");
	h->start_server();
	return h;
}

// start remote ref
int client_start(remote_class_t *obj)
{
	hcomm_t h("127.0.0.1", 5557, "client proc");
	h.connect();

	remote_class_tstub_t *st = new remote_class_tstub_t(&h, "remote_obj");

	if (strcmp("foo test call!", st->foo()))
	{
		printf("foo call fail\n");
		st->close();
		return 0;
	}
/*	if (st->foo1(4)!=42)
	{
		printf("foo1 call fail\n");
		st->close();
		return 0;
	}

	bar_t bar;
	bar1_t c;
	c.c = 42;
	bar.a = c;
	bar.b = 0;
/*
	if (st->foo2(&bar)!=1488)
	{
		printf("foo2 call fail\n");
		st->close();
		return 0;
	}

	if (*st->foo3(bar)!=1488)
	{
		printf("foo3 call fail\n");
		st->close();
		return 0;
	}

	bar2_t bar2;
	bar2.c = new bar1_t;
	bar2.c->c = 41;
	bar2.d.push_back(1);

	if (st->foo4(&bar2)!=1488)
	{
		printf("foo4 call fail\n");
		st->close();
		return 0;
	}

	vector<char*> vbf = st->foo5();
	if (strcmp("sieg", vbf[0]))
	{
		printf("foo5 call fail\n");
		st->close();
		return 0;

	}

	vector<char*> *vbf1 = st->get_posts("a","b");
	if (strcmp("sieg", vbf1->at(0)))
	{
		printf("foo6 call fail\n");
		st->close();
		return 0;
	}

	bar3_t *vbf2 = st->foo7();
	if (vbf2->d->at(0)!=42)
	{
		printf("foo7 call fail\n");
		st->close();
		return 0;
	}
*/
	st->close();
	return 1;
}

#endif
// do test
void ltest_call()
{
#ifndef REGEN

	hcomm_srv_t ns;
	ns.start_server(5557, 5510, 5520);
	sleep(2);

	remote_class_t *obj = new remote_class_t;

	hcomm_t *srv_mdlw = remote_obj_start(obj);
	sleep(2);
	TS_ASSERT_EQUALS(1, client_start(obj));
	srv_mdlw->kill_server();
	sleep(2);
//	ereport_t *r = ns.getrep();

//	cout << r->get();

	ns.kill_server();
#endif
}

};

//
