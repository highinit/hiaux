/* 
 * File:   hconfig_parser.h
 * Author: phrk
 *
 * Created on May 16, 2013, 11:42 AM
 */

#ifndef HCONFIG_PARSER_H
#define	HCONFIG_PARSER_H

#include <map>
#include <string>
#include <vector>

using namespace std;

class HconfigParser
{
public:
    
    static bool existVar(map<string,string>* vars, string key)
    {
        map<string,string>::iterator it = vars->find(key);
        if (it==vars->end())
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    
    static bool existVars(map<string,string>* vars, vector<string> keys)
    {
        for (int i = 0; i<keys.size(); i++)
        {
            if (!existVar(vars, keys[i]))
            {
                return 0;
            }
        }
        return 1;
    }
    
    static map<string,string>* load(string file_name)
    {
        map<string,string>* vars = new map<string,string>;
        FILE *f = fopen(file_name.c_str(), "r");
        while (!feof(f))
        {
            char key[255];
            char value[255];
            char bf[255];
            fgets(bf, 255, f);
            sscanf(bf, "%s %s", key, value);
            vars->insert(pair<string,string>(string(key), string(value)));
        }
        fclose(f);
        return vars;
    }
};

#endif	/* HCONFIG_PARSER_H */

