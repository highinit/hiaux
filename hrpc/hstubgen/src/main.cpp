//#include <iostream>

//#include "typet.h"
#include "stubgen.h"
using namespace std;
//#include "hfss.h"
//#include "hfssstubc_t.h"

//extern int subi;

bool stubs = 1;
//extern int tabs = 0;

int main (int argc, char *argv[])
{	 
	printf( "Vardist stubgen! \n");
	if (argc<4) 
	{
		 printf("Usage:	 classname in_header_file out_skelfile\n");
//		 printf("	-json classname in_header_file out_file\n");
		 return 0;
	}
	stubgen_t *s = new stubgen_t;
	s->gen(argv[1], argv[2], argv[3]);
//	s->gen("hfs_t", "hfss.h", "skelhfs.h");
	delete s;
/*	
	if  (!strcmp(argv[1], "-json"))
	{
		printf("JSON generating\n");
		stubgen_t *s = new stubgen_t;
		stubs = 0;
		s->gen_json(argv[2], argv[3], argv[4]);
		delete s;
	}
	else 
	{
		printf("Stub and Skel generating\n");
		stubgen_t *s = new stubgen_t;
		stubs = 1;
		s->gen(argv[1], argv[2], argv[3]);
		s->list_all_types();

		//s->list_type("vector<char*>");
		delete s;
		
		s = new stubgen_t;
		stubs = 0;
		//s->list_type("read_ret_t*");
		s->gen(argv[1], argv[2], argv[4]);
		delete s;
	} 
	
*/
	return 0;
}

