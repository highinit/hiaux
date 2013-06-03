/* 
 * File:   tests.cpp
 * Author: phrk
 *
 * Created on May 28, 2013, 11:58 PM
 */

#include <cstdlib>
#include <iostream>
#include "hlogger.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) 
{
    try
    {
    
        Hlogger *logger = new Hlogger("127.0.0.1", 27017, "highinit_test", "logs", \
                "worker1", "dbuser", "dbuser");

        logger->log(EVENT0, "hcomm_t::connect");
        logger->log(EVENT1, "hcomm_t::connect");
        logger->log(WARNING, "hcomm_t::connect WARNiNG");
        logger->log(ERROR, "hcomm_t::connect ERROR");
    
    }
    catch (string *s)
    {
        cout << s->c_str();
    }
    
    return 0;
}

