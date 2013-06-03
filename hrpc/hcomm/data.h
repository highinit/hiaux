#pragma once
/*
#include <cstdio>
#include <iostream>
#include <cstring>
#include <cstdlib>
*/
#include "hcomm.h"

class data_t
{
	public:
	
	//#remote
	char* request(char *a)
	{
		cout << a << endl;
		return "OK";
	}
};
