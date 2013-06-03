#include <iostream>
//#include "hcomms.h"
//#include "data.h"
#include "common.h"
#include "hcomm.h"
//#include "hdatastub.h"
using namespace std;
//#include "unistd.h"
//#include "sys/types.h"

int main (int argc, char *argv[])
{
	/*
	printf( "VarDist nameserver \n");
	if (argc!=4) 
	{ 
		 printf("Usage: srvport cliport_begin cliport_end\n");
		 return 0;
	}
	
	hcomm_srv_t s;
	s.start(strtoint(argv[1]), strtoint(argv[2]), strtoint(argv[3]));
	*/
	
//	log_t::openlog("out.txt");
//	log("start ");
	cout << "hcomm dev project!" << endl;
	
	hcomm_srv_t s;
	s.start(5559, 5520, 5525);
	
	return 0;
}

