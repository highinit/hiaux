//
//  hlog.cpp
//  
//
//  Created by Artur Gilmutdinov on 4/26/13.
//
//

#ifndef _hlog_cpp
#define _hlog_cpp

#include "../include/hlog.h"

void hlog_clear(string file)
{
    FILE *f = fopen(file.c_str(), "w");
}

void hlog_log(string s, string file)
{
    FILE *f = fopen(file.c_str(), "a+");
    fprintf(f, "%s", s.c_str());
    fprintf(f, "\n");
    fclose(f);
}



#endif
