#pragma once
#include "incl.h"
#include "commonstub.h"

#define STUB 0
#define SKEL 1
#define TOJS 2
#define FROMJS 3


using namespace std;

class type_t 
{
	public:
	// all known
	vector<type_t*> *type;
	bool vec;
	char name[255];
	// -1 if not
	int basetype;
	// type / name
	vector<char*>  fields;
	vector<char*> fnames;
	vector<char*> ps;
	type_t();
	
	// new
	bool isbasetype()
	{
		if (fields.size()==0 && !vec) return 1;
		return 0;
	}

	type_t *gettype(const char *nm)
	{
		for (int i =0; i<type->size(); i++)
			if (!strcmp(type->at(i)->name, nm)) return type->at(i);
		return NULL;
	}

	// 
	void cstub( const char *last1, const char *vname1, FILE *out, int subi, int mode, bool inarr);
	
	type_t(int t, const char *nm, vector<type_t*> *tp);
	
	void addfield(const char *fl, const char *nm);
	
	char *invec();
	bool ifp(char *b);
	void ctype(const char *last, const char *tname, const char *vname, int subi, int mode, bool inarr);
	void jsontype(char *last, char *tname, char *vname, int subi);
	void cbasetype(const char *last, const char *vname, int mode);
	
	
	void sstub(FILE *out);
};


