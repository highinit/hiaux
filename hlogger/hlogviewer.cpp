/* 
 * File:   hlogviewer.cpp
 * Author: phrk
 *
 * Created on May 29, 2013, 12:00 AM
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
using namespace std;

#include "hlogger.h"
#include "hlogviewer.h"

#include "hlogviewer_cli.h"

int main(int argc, char** argv) 
{
    HlogViewer* logviewer = new HlogViewer("127.0.0.1", 27017, argv[1], \
            "logs", "dbuser", "dbuser");
    
    HlogViewerConsole* hlogviewer_cli = new HlogViewerConsole(logviewer);     
    Hconsole *hconsole = new Hconsole(hlogviewer_cli);
    hconsole->run();
    
    return 0;
}

