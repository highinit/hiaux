#include "../../hcomm/include/sendchannel.h"
#include "../../hcomm/include/hqueue.h"
#include <cxxtest/TestSuite.h>

#define PORT 13048

template <class type>
class ch_test_thread_arg_t
{
    public:
    type *obj;
};

void* ch_test_thread_f(void* arg);

class sendch_test_suite : public CxxTest::TestSuite
{
	public:
    send_channel_t *ch_s;
    sem_t sem;
    hsock_t hsock;
    pthread_t th;
    
    string bf;
    string bf2;
    string bf3;
    string bf4;
    string bf5;

    virtual void setUp()
    {

    }

    void start_server()
    {
        try
        {
                ch_s = new send_channel_t(send_channel_t::hsock.simple_server(PORT));
                sleep(10);
                ch_s->csend("test4");
                bf = ch_s->crecv();
                bf2 = ch_s->crecv();
                bf3 = ch_s->crecv();
                bf5 = ch_s->crecv();
                //sleep(4);
                ch_s->close();
                //send_channel_t::close_server(PORT);
        }
        catch (string *s)
        {
            cout << "start server thread: "  << *s;
        }
        pthread_detach(th);
    }

    void ltestQueue()
    {
        hqueue_t<int> q;
        q.push(1);
        q.push(2);
        q.push(3);
        TS_ASSERT_EQUALS(1,q.pop());
        TS_ASSERT_EQUALS(2,q.pop());
        TS_ASSERT_EQUALS(3,q.pop());
    }

    void testSendChannel()
    {
        try
        {
        ch_test_thread_arg_t<sendch_test_suite> *arg = new ch_test_thread_arg_t<sendch_test_suite>;
        arg->obj = this;
       
        pthread_create(&th, NULL, ch_test_thread_f, arg);
        send_channel_t *ch_c;

        
        

        ch_c = new send_channel_t();
        ch_c->connect("127.0.0.1", PORT);
        
        
        ch_c->csend("test");
        ch_c->csend("test2");
        ch_c->csend("test3");
        ch_c->csend("test5");
        
        
        bf4 = ch_c->crecv();

        cout << bf << endl << bf2 << endl << bf3 << endl << bf4 << endl << bf5;
        
        ch_c->close();
        
        
        TS_ASSERT_EQUALS(1, "test" == bf);
        TS_ASSERT_EQUALS(1, "test2" == bf2);
        TS_ASSERT_EQUALS(1, "test3" == bf3); 
        TS_ASSERT_EQUALS(1, "test4" == bf4);
        TS_ASSERT_EQUALS(1, "test5" == bf5);
        
        } catch (string *s)
        {
            cout << *s;
        }
        /*
        ch_s->csend("test2\n");
//        TS_ASSERT_EQUALS(OK, ch_s->check_fault());
//        TS_ASSERT_EQUALS(OK, ch_c->check_fault());
        char *bf1 = ch_c->crecv();
        TS_ASSERT_EQUALS(1, !strcmp("test2\n", bf1));

        ch_s->close();
        //sleep(10);
 //       TS_ASSERT_EQUALS(FAULT, ch_c->check_fault());
 //       TS_ASSERT_EQUALS(FAULT, ch_s->check_fault());
        ch_c->close();

         
        delete [] bf;
        delete [] bf1;
        delete ch_s;
        */
        
        
    }

};

void* ch_test_thread_f(void* arg)
{
    ch_test_thread_arg_t<sendch_test_suite> *newarg = (ch_test_thread_arg_t<sendch_test_suite>*) arg;
    newarg->obj->start_server();
}



