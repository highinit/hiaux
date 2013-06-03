#pragma once
//#include <vector>
//#include <iostream>
//#include <fstream>
//#include "string.h"
#include "incl.h"
//#include "stdio.h"


void puttabs(const int i);

bool instring(const char *what,const char *wher);

bool firststr(const char *what, const char *wheres);
bool ischar(char *bf);

int firsts(char t, char *bf);

bool methodthere(char *shit, int *counter);

bool fieldthere(char *shit, int *counter);

// char *a, char* a
void movestar(char *shit);

void backstar(char *shit);

void removespaces(char *shit);
const char *nop(const char *bf);
bool isp(char *bf);
void firstspace(char *shit);
void lastspace(char *shit);
void tospace(char t, char *bf);
void next(char *bf, char *to);

// -> alternative
char *addptolast(char *l, char *n);

// . implementation
char *addtolast(const char *l, const char *n);

int findfirst(char *a, char t);
char* inside_vec(const char *type);
bool typeqv(const char *a, const  char *b);
void add_to_str(pair<char*, int>* a, pair<char*, int>* b);

// add until '\0'
void add_str_to_str(pair<char*, int>* a, char* b);

char *gen_func_by_type(const char *type, const char* fgclass);
char *neg_func_by_type(const char *type, const char* fgclass);
