#pragma once
#include "incl.h"
#include "typet.h"
#include "commonstub.h"
#include <map>
#include "../../hcomm/include/objrepr.h"


#define LINE(a) str.push_back(newline( a ))

// gens serializ functions
class repr_gen
{
	static vector<char*> generated;
	static vector<char*> neged;
	public:

	static void init()
	{
	add_as_gened("void");
	add_as_gened("int");
	add_as_gened("char*");
	add_as_gened("char*");
	add_as_gened("char");
	add_as_gened("float");
	
	add_as_neged("void");
	add_as_neged("int");
	add_as_neged("char*");
	add_as_neged("char*");
	add_as_neged("char");
	add_as_neged("float");
	}

	static char *newline(const char *str)
	{
		char *bf = new char [255];
		strcpy(bf, str);
	return bf;
	}

	static bool already_gened(const char *type)
	{
		for (int i =0 ; i<generated.size(); i++)
			if (!strcmp(generated[i], type)) return 1;
		return 0;
	}

	static bool already_neged(const char *type)
	{
		for (int i =0 ; i<neged.size(); i++)
			if (!strcmp(neged[i], type)) return 1;
		return 0;
	}

	static void add_as_gened(const char *type)
	{
		generated.push_back(newline(type));
	}

	
	static void add_as_neged(char *type)
	{
		neged.push_back(newline(type));
	}

	static void add_to_vec(vector<char*> *a, vector<char*> *b)
	{
		for (int i = 0; i<b->size(); i++)
			a->push_back(b->at(i));
	}

	static pair<char*, int>* gen_vec_func(type_t *type);
	
	static pair<char*, int>* neg_vec_func(type_t *type);

	static pair<char*, int>* gen_func(type_t *type);

	static pair<char*, int>* neg_func(type_t *type);

	// gen gen functions. deep search in type tree
	static pair<char*, int>* gen_all(type_t *type)
	{
	pair<char*, int>* ret = new pair<char*, int>(new char [1], 0);
	if (already_gened(nop(type->name))) return ret;	

	for (int i = 0; i<type->fields.size(); i++)
			if (!already_gened(nop(type->fields[i])))
			{
			add_to_str(ret, gen_all(type->gettype(type->fields[i])));
			add_as_gened(type->fields[i]);
			}
		
		pair<char*, int> *bf = gen_func(type);
		add_to_str(ret, bf);
	add_as_gened(type->name);
	return ret;
	}

	// gen neg functions
	static pair<char*, int>* neg_all(type_t *type)
	{
	pair<char*, int>* ret = new pair<char*, int>(new char [1],0);
	if (already_neged(nop(type->name))) return ret;	

	for (int i = 0; i<type->fields.size(); i++)
			if (!already_neged(nop(type->fields[i])))
			{
			add_to_str(ret, neg_all(type->gettype(type->fields[i])));
			add_as_neged((char*)nop(type->fields[i]));
			}
		
		pair<char*, int> *bf = neg_func(type);
		add_to_str(ret, bf);
	add_as_neged((char*)nop(type->name));
	return ret;
	}

};
