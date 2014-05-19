/*
 * Copyright (c) 2010-2013  Artur Gilmutdinov

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the BSD 2-Clause License

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * BSD 2-Clause License for more details.

 * You should have received a copy of the BSD 2-Clause License
 * along with this program; if not, see <http://opensource.org/licenses>.

 * See also <http://highinit.com>
*/

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
            //std::cout << "t1:" << t1 << " t2:" << t2 << " t3:" << t3 << " t4:" << t4 << std::endl;
            if (t1==t2 && t2==t3 && t3==t4 && t4==42) {
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
      
       pool.addTask( NEWTASK2(&Tasks::f1, &tasks));
       pool.addTask( NEWTASK2(&Tasks::f2, &tasks));
       pool.addTask( NEWTASK2(&Tasks::f3, &tasks));
	   pool.addTask( NEWTASK2(&Tasks::f4, &tasks));
       pool.run();
       
       sleep(1);
       
       TS_ASSERT(tasks.check()==1);
    }
    
    void test1()
    {
       Tasks tasks;
       hThreadPool pool(2);
       pool.run();
       
       pool.addTask( NEWTASK2(&Tasks::f1, &tasks));
       pool.addTask( NEWTASK2(&Tasks::f2, &tasks));
       pool.addTask( NEWTASK2(&Tasks::f3, &tasks));
       pool.addTask( NEWTASK2(&Tasks::f4, &tasks));
       
	   sleep(1);
       TS_ASSERT(tasks.check()==1);
    }
};