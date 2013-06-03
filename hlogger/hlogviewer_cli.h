/* 
 * File:   hlogviewer_cli.h
 * Author: phrk
 *
 * Created on May 30, 2013, 5:43 AM
 */

#ifndef HLOGVIEWER_CLI_H
#define	HLOGVIEWER_CLI_H

#include "hlogviewer.h"
#include "hconsole.h"

class HlogViewerConsole: public HconsoleClient
{
    HlogViewer* logviewer;
public:
    
    HlogViewerConsole(HlogViewer* logviewer)
    {
        this->logviewer = logviewer;
    }
    
    virtual vector<string> interpret(const vector<string> params)
    {
        vector<string> output;
        string node = params [0];
        string pattern = params [1];
        vector<LogEvent> events = logviewer->doQuery(node, 0, pattern, 10);
        for (int i = 0; i<events.size(); i++) {
            string line = events [i].node + " " + events [i].event;
            if (events[i].event.find(pattern)!=std::string::npos \
                    || pattern == "all") {
                output.push_back(line);
            }
        }
        return output;
    }
    
};

#endif	/* HLOGVIEWER_CLI_H */

