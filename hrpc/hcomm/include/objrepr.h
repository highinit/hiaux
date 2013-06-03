#pragma once
#include "string.h"
#include <vector>
#include "stdio.h"
#include "sendchannel.h"

using namespace std;

class objrepr_t
{
public:



	string type;
	// if basetype
	string value;
	// field name
	vector<pair<string, objrepr_t*> > fields;

	objrepr_t()
	{
        /*        type = new char [255];
                value = new char [255];
	strcpy(type, "");
	strcpy(value, "");*/
	}

	objrepr_t(string t, string v)
	{
	//type = new char [255];
	//value = new char [255];
	//strcpy(type, t);
	//strcpy(value, v);
            type = t;
            value = v;
	}

	objrepr_t *getfield(string n)
	{
		for (int i =0; i<fields.size(); i++)
		{
			//if (!strcmp(fields[i].first, n)) return fields[i].second;
                    if (fields[i].first==n) return fields[i].second;
		}
		return NULL;
	}

	~objrepr_t()
	{
        //delete [] type;
        //delete [] value;
        //type.clear();
        //value.clear();
        /*for (int i = 0; i<fields.size(); i++)
        {
            //delete [] fields[i].first;
            //fields[i].first.clear();
            delete fields[i].second;
        }*/
             //   fields.clear();
	}

};


	objrepr_t *int_gen(int *a);
	objrepr_t *float_gen(float *a);
	objrepr_t *char_gen(char *a);

	int *int_neg(objrepr_t *a);
	float *float_neg(objrepr_t *a);
	string *char_neg(objrepr_t *a);


bool cmp_objrepr(objrepr_t *a, objrepr_t *b);
void objrepr_send(objrepr_t *obj, send_channel_t *ch, bool first);
objrepr_t *objrepr_recv(send_channel_t *ch, bool first);

