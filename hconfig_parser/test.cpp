/* 
 * File:   test.cpp
 * Author: phrk
 *
 * Created on May 16, 2013, 11:42 AM
 */

#include <cstdlib>
#include <iostream>
using namespace std;


#include "hconfig_parser.h"
/*
 * 
 */
int main(int argc, char** argv) 
{
    map<string,string> *vars = HconfigParser::load("config.conf");
    vector<string> keys; 
    keys.push_back("shitty_var_1");
    keys.push_back("shity_var2");
    if (HconfigParser::existVars(vars, keys))
    {
        cout << "OK";
    }
    else
    {
        cout << "HconfigParser::existVars FAIL";
    }
    

    return 0;
}

