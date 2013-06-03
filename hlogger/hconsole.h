/* 
 * File:   hconsole.h
 * Author: phrk
 *
 * Created on May 30, 2013, 5:43 AM
 */

#ifndef HCONSOLE_H
#define	HCONSOLE_H

#include <iostream>
#include <sstream>
using namespace std;

vector <string> split_string(const string s)
{
    vector <string> ret;
    istringstream iss(s);
    do {
        string sub;
        iss >> sub;
        if (sub!="") {
            ret.push_back(sub);
        }
    } while (iss);
    return ret;
}

class HconsoleClient
{
public:
    virtual vector<string> interpret(const vector<string> commands) = 0;
};


class Hconsole
{
    vector<string> command_stack;
    HconsoleClient *client;
        
public:
    
    Hconsole(HconsoleClient *client)
    {
        this->client = client;
    }
    
    void run()
    {
        
        cout << "hlogs>:";
        while (1) {
            char c;
            cin.get(c);

            if (c == 'q') {
                cout << "quiting";
                exit(0);
            }
            
            char bf[256];
            cin.getline (bf, 256);
            vector <string> params = split_string(string(bf));
                    
            vector<string> output = client->interpret(params);
            for (int i = 0; i<output.size(); i++) {
                cout << output [i] << endl;
            }
            
            cout << "hlogs>:";
        }
    }
    
};

#endif	/* HCONSOLE_H */

