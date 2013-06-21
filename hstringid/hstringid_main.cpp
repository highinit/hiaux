/* 
 * File:   hstringid_main.cpp
 * Author: phrk
 *
 * Created on June 16, 2013, 1:10 PM
 */

#include <cstdlib>

#include "hstringid_worker.h"

using namespace std;

int main(int argc, char** argv) 
{
    try
    {
        HstringIdWorker::start(HconfigParser::load(argv[1]));
    }
    catch (string *s)
    {
        cout << s->c_str();
    }
    return 0;
}

