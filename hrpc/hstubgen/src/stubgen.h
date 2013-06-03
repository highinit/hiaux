
#pragma once
#pragma warning(disable:4018)
#pragma warning(disable:4996)



#include "incl.h"

//#include "typet.h"

#include "extract_class.h"
#include "objreprgen.h"

using namespace std;


#define cout fout

extern int subi;
extern ofstream fout;

typedef vector<pair<char*,char*> > vecpair_t;

class stubgen_t
{
	public:

	extracter_t *extracter;

	char *fname;
	vector<type_t*> *type;
	vector<method_t*> method;
	char *fgclass;

	stubgen_t()
	{
		type = new vector<type_t*>;
		extracter = new extracter_t(type);
	}

	~stubgen_t()
	{
	}

	void list_all_types()
	{
		for (int i = 0; i<type->size(); i++)
		{
		printf("%s:\n", (*type)[i]->name);
			for (int j = 0; j<(*type)[i]->fields.size(); j++)
			{
			printf("	%s %s\n", (*type)[i]->fields[j], (*type)[i]->fnames[j]);
			}
		}
	}

	vecpair_t* get_type(char *nm)
	{
	// fieldtype / fieldname
		vecpair_t *field = new vecpair_t;
		for (int i = 0; i<type->size(); i++)
		if (!strcmp(nm, (*type)[i]->name))
		{
			for (int j = 0; j<(*type)[i]->fields.size(); j++)
			{
			field->push_back(pair<char*,char*>((*type)[i]->fields[j], (*type)[i]->fnames[j]));
			}
		return field;
		}
	}

	void list_type(char *nm)
	{
	printf("listing: %s\n", nm);
	for (int i = 0; i<type->size(); i++)
	if (!strcmp(nm, (*type)[i]->name))
		{
			for (int j = 0; j<(*type)[i]->fields.size(); j++)
			{
			printf("%s %s\n", (*type)[i]->fields[j], (*type)[i]->fnames[j]);
			}
		return;
		}
	}

	int nparams(char *mname)
	{
		for (int i =0; i<method.size(); i++)
		if (!strcmp(mname, method[i]->name))
		return i;
	return 0;
	}

/*	bool typexist(const char *nm)
	{
	for (int i = 0; i<type->size(); i++)
		if(!strcmp(nm, (*type)[i]->name))
			return 1;
	return 0;
	}
*/
	void read_target(char *classname, char *fname);

	void gen(char *classname, char *fname, char *outf);
	void gen_stub(const char *classname);
	void gen_skel(const char *classname);
//	void gen_json(char *classname, char *inf, char *outf);
};
