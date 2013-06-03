
#pragma once

#include "../meroving/hcomm/include/common.h"
#include "../meroving/hcomm/include/hcomm.h"
#include "../meroving/hcomm/include/hcomms.h"
#include "master/hword_master.h"

class Hword_fg
{
 public: 
	static objrepr_t *int_gen(int64_t a)
	{
		objrepr_t *repr = new objrepr_t;
                char bf[15];
                strcpy(bf, "");
		sprintf(bf, "%lli", a);
                repr->value = string(bf);
		repr->type = "int";
                //delete a;
		return repr;
	}

	static objrepr_t *float_gen(float *a)
	{
		objrepr_t *repr = new objrepr_t;
		char bf[15];
                strcpy(bf, "");
		sprintf(bf, "%f", *a);
                repr->value = bf;
		repr->type = "float";
		return repr;
	}

	static objrepr_t *char_gen(string *a)
	{
		objrepr_t *repr = new objrepr_t;
		repr->value = *a;
		repr->type = "string";
		return repr;
	}

	static int64_t int_neg(objrepr_t *repr)
	{
                int64_t r;
		sscanf(repr->value.c_str(), "%lli", &r);
		delete repr;
		return r;
	}

	static float *float_neg(objrepr_t *repr)
	{
		float *r = new float;
		sscanf(repr->value.c_str(), "%f", r);
		delete repr;
		return r;
	}

	static string *char_neg(objrepr_t *repr)
	{
		string *r = new string;
		*r = repr->value;
		delete repr;
		return r;
	}

        static objrepr_t* string_gen(string obj)
        {
                objrepr_t* ret = new objrepr_t("string", obj);
                return ret;
        }


        static string string_neg(objrepr_t* repr)
        {
                string ret;
                ret = repr->value;
                delete repr;
                return ret;
        }


};

class HwordMasterStub: public HwordMasterIfs
{
	char bf[255];
	int bfi; 
 	float bff;     
 public:   
         send_channel_t *ch;

		void close() 
		{
			ch->csend("ifs stop"); 
                        ch->close();
		}

		HwordMasterStub(hcomm_t *c, string name) 
 		{
 			c->open_ifs<HwordMasterStub>(this, name); 
 		} 

		
		int64_t getId(string word)
		{ 
			ch->csend("getId");
			objrepr_send(Hword_fg::string_gen(word), ch, 1);
                        int64_t id = Hword_fg::int_neg(objrepr_recv(ch, 1));
			ch->crecv();
                        return id;
		} 
		//End remote function 
};

// Server skel for HwordMaster
class HwordMasterSkel : public skel_t
{
	HwordMaster *obj;

	char bf[255]; 
 	int bfi; 
 	float bff;
	public: 

	HwordMasterSkel(HwordMaster *o) { obj = o; }

	virtual char *call(send_channel_t* ch, string &command) 
	{ 
		//remote function (#remote) 
		if (command == "getId") 
		{ 
			string word = Hword_fg::string_neg(objrepr_recv(ch, 1));
			int64_t id = obj->getId(word);
			objrepr_send(Hword_fg::int_gen(id), ch, 1);

		return 0;
		} 
		//End remote function 
	}
 };
