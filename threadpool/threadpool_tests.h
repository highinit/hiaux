
#include <cxxtest/TestSuite.h>

#include "threadpool.h"

class Tasks
    {
        int t1;
        int t2;
        int t3;
        int t4;
    public:
        
        void f1()
        {
            t1 = 42;
        }
        void f2()
        {
            t2 = 42;
        }
        void f3()
        {
            t3 = 42;
        }
        void f4()
        {
            t4 = 42;
        }
        
        bool check()
        {
            //std::cout << t1 << " " << t2 << " " << t3 << " " << t4 << " \n" ;
            if (t1==t2 && t2==t3 && t3==t4 && t4==42)
            {
                return 1;
            }
            return 0;
        }
        
    };

    
class HdividerDistTests : public CxxTest::TestSuite
{    

public:
    
    
    
    void testO()
    {
       Tasks tasks;
       hThreadPool pool(2);
      
       pool.addTask( boost::bind(&Tasks::f1, &tasks));
       pool.addTask( boost::bind(&Tasks::f2, &tasks));
       pool.addTask( boost::bind(&Tasks::f3, &tasks));
       pool.addTask( boost::bind(&Tasks::f4, &tasks));
       pool.run();
       
       sleep(1);
       
       TS_ASSERT(tasks.check()==1);
        
    }
    
    void test1()
    {
       Tasks tasks;
       hThreadPool pool(2);
       pool.run();
       sleep(1);
       pool.addTask( boost::bind(&Tasks::f1, &tasks));
       pool.addTask( boost::bind(&Tasks::f2, &tasks));
       pool.addTask( boost::bind(&Tasks::f3, &tasks));
       pool.addTask( boost::bind(&Tasks::f4, &tasks));
       
       sleep(1);
       
       
       TS_ASSERT(tasks.check()==1);
        
    }
};