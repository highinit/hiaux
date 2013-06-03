#pragma once
#include "incl.h"
#include "commonstub.h"
#include "method.h"

#define INT 0
//#define CHAR 1
//#define CHARP 2
#define STR 1
#define FLOAT 3
#define VOID 4
#define UNDEF -1


class extracter_t
{
	static vector<char*> include;
	
	char *fname;
	vector<type_t*> *type;
	//vector<method_t*> method;

	public:

	extracter_t(vector<type_t*> *type)
	{
		fname = new char [255];
		this->type = type;
		type_t *t = new type_t(INT, "int", type);
		type->push_back(t);
		t = new type_t(STR, "string", type);
		type->push_back(t);
		//t = new type_t(CHARP, "char*", type);
		//type->push_back(t);
		t = new type_t(FLOAT, "float", type);
		type->push_back(t);
		t = new type_t(VOID, "void", type);
		type->push_back(t);
	}

	void setfname(const char *nm)
	{
		strcpy(fname, nm);
	}

	int basetype(const char *bf);
	
	void find_type_infile(const char *tname);
	void find_type(const char *tname1, const char *varname);
	
	bool typexist(const char *nm)
	{
	for (int i = 0; i<type->size(); i++)
		if(!strcmp(nm, (*type)[i]->name))
			return 1;
	return 0;	
	}
	
};
