#include "../include/objrepr.h"
#include "string.h"
#include "stdio.h"


	objrepr_t *int_gen(int *a)
	{
		objrepr_t *repr = new objrepr_t;
                char bf[15];
                strcpy(bf, "");
		sprintf(bf, "%d", *a);
                repr->value = bf;
		repr->type = "int";
		return repr;
	}

	objrepr_t *float_gen(float *a)
	{
		objrepr_t *repr = new objrepr_t;
		char bf[15];
                strcpy(bf, "");
		sprintf(bf, "%f", *a);
                repr->value = bf;
		repr->type = "float";
		return repr;
	}

	objrepr_t *char_gen(string *a)
	{
		objrepr_t *repr = new objrepr_t;
		repr->value = *a;
		repr->type = "string";
		return repr;
	}

	int *int_neg(objrepr_t *repr)
	{
		int *r = new int;
		sscanf(repr->value.c_str(), "%d", r);
		delete repr;
		return r;
	}

	float *float_neg(objrepr_t *repr)
	{
		float *r = new float;
		sscanf(repr->value.c_str(), "%f", r);
		delete repr;
		return r;
	}

	string *char_neg(objrepr_t *repr)
	{
		string *r = new string;
		*r = repr->value;
		delete repr;
		return r;
	}



bool cmp_objrepr(objrepr_t *a, objrepr_t *b)
{
	if (a->type != b->type) return 0;
	if (a->value != b->value) return 0;
	if (a->fields.size()!=b->fields.size()) return 0;

	if(a->fields.size()!=b->fields.size()) return 0;

	for (int i =0; i<a->fields.size(); i++)
	{
		if (a->fields[i].first != b->fields[i].first) return 0;
		if (! cmp_objrepr(a->fields[i].second, b->fields[i].second) ) return 0;

	}

return 1;
}


void objrepr_send(objrepr_t *obj, send_channel_t *ch, bool first)
{
	ch->csend(obj->type);
	ch->csend(obj->value);
        char bf[255];
	sprintf(bf, "%d\n", (int)obj->fields.size());
	ch->csend(string(bf));
	for (int i =0; i<obj->fields.size(); i++)
	{
		ch->csend(obj->fields[i].first);
		objrepr_send(obj->fields[i].second, ch, 0);
	}
//	if (first) ch->finish();
	delete obj;
}

objrepr_t *objrepr_recv(send_channel_t *ch, bool first)
{

//	if (first) ch->start();
	objrepr_t *obj = new objrepr_t();
	
	obj->type = ch->crecv();
	obj->value = ch->crecv();

	string bf = ch->crecv();
	int n;
	sscanf(bf.c_str(), "%d", &n);

	for (int i = 0; i<n; i++)
	{
		string bf = ch->crecv();
		obj->fields.push_back(pair<string, objrepr_t*>(bf, objrepr_recv(ch, 0)));
	}

return obj;
}
