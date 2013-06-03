#pragma once
#include "incl.h"
#include "typet.h"

class method_t
{
	public:
	vector<type_t*> *type;

	char name[255];
	vector<char*> params;
	vector<char*> names;
	vector<char*> ps;
	char* ret;
	bool retpointer;

	void gen_send_method(FILE *out, char* fgclass);
	void gen_recv_method(FILE *out, char* fgclass);

	method_t()
	{
		type = new vector<type_t*>;
	}
	~method_t()
	{

	}

	method_t(char *bf, vector<type_t*> *t)
	{
			type = t;
			strcpy(name, bf);
	}

	void ctype(const char *tname,const char *vname);

	void sstub(FILE *out)
	{

	}
};
