#include "objreprgen.h"

vector<char*> repr_gen::generated;
vector<char*> repr_gen::neged;

// vec gen
pair<char*, int>* repr_gen::gen_vec_func(type_t *type)
	{
		// if vector
		char bf[255];
		pair<char*, int>* ret = new pair<char*, int>;
		vector<char*> str;


		sprintf(bf, "   static objrepr_t* %s(%s *obj)\n {\n", gen_func_by_type(type->name, NULL), type->name);
		LINE(bf);

		type_t *curfield;

		sprintf(bf, "       objrepr_t* ret = new objrepr_t(\"%s\",\"\");\n", type->name);
		LINE(bf);
		sprintf(bf, "       objrepr_t* elem;\n");
		LINE(bf);

		sprintf(bf, "       for (int i = 0; i<obj->size(); i++) \n	{ \n");
		LINE(bf);

		char inside[255];
		strcpy(inside, nop(inside_vec(type->name)));
		type_t *invectype = NULL;


		invectype = type->gettype(inside);



			if (type->ifp(invectype->name) || !strcmp(invectype->name, "char"))
			{

			sprintf(bf, "       ret->fields.push_back(pair<char*, objrepr_t*>(\"%s\", %s(obj->at(i))));\n",invectype->name, \
					gen_func_by_type(invectype->name, NULL));
			}
			else
			{
				sprintf(bf, "       ret->fields.push_back(pair<char*, objrepr_t*>(\"%s\", %s(&obj->at(i))));\n",invectype->name, \
					gen_func_by_type(invectype->name, NULL));
			}
			LINE(bf);

		sprintf(bf, "       }\n");
		LINE(bf);


		LINE("  return ret;\n");
		LINE("  }\n\n");

		int len = 0;
		for (int i = 0; i<str.size(); i++)
			len += strlen(str[i]);
		ret->first = new char [len];
		ret->second = len;
		strcpy(ret->first, "");

		for (int i = 0; i<str.size(); i++)
		{
			strcat(ret->first, str[i]);
			delete [] str[i];
		}

		return ret;

	}

	// if not vector
pair<char*, int>* repr_gen::gen_func(type_t *type)
	{
		pair<char*, int>* ret = new pair<char*, int>;
		vector<char*> str;


	//	str.push_back(newline("HEIL\n"));  eqv LINE("HEIL\n")

		if (!strncmp(type->name, "vector<", 6))
		{
			//add_to_vec(&str,
			return	gen_vec_func(type);
			//goto GOTO_label_return;
		}

		char bf[255];
		sprintf(bf, "   static objrepr_t* %s(%s *obj)\n  {\n", gen_func_by_type(type->name, NULL), type->name);
		LINE(bf);


		type_t *curfield;

		sprintf(bf, "       objrepr_t* ret = new objrepr_t(\"%s\",\"\");\n", type->name);
		LINE(bf);
		sprintf(bf, "       objrepr_t* curf;\n");
		LINE(bf);
		for (int i = 0; i<type->fields.size(); i++)
		{
			curfield = type->gettype(type->fields[i]);
			// basetype
		/*	if (curfield->isbasetype())
			{
				if (!strcmp(curfield->name, "int"))
				{
				sprintf(bf,"	curf = new objrepr_t(\"int\", inttostr(obj->%s));\n", type->fnames[i]);
				}
				else if (!strcmp(curfield->name, "float"))
				{
				sprintf(bf,"	curf = new objrepr_t(\"float\", floattostr(obj->%s));\n", type->fnames[i]);
				}
				else if (!strcmp(curfield->name, "char") || !strcmp(curfield->name, "char*") \
					|| !strcmp(curfield->name, "char *"))
				{
				sprintf(bf,"	curf = new objrepr_t(\"char*\", newstr(obj->%s));\n", type->fnames[i]);
				}
			LINE(bf);
			sprintf(bf, "	ret->fields.insert(pair<char*, objrepr_t*>(\"%s\", curfield));\n",type->fnames[i]);
			LINE(bf);
			}
			// not basetype
			else*/
			{
			if (type->ifp(curfield->name) || isp(type->fnames[i]))
			sprintf(bf, "       ret->fields.push_back(pair<char*, objrepr_t*>(\"%s\", %s(obj->%s)));\n", nop(type->fnames[i]), \
					gen_func_by_type(curfield->name, NULL), nop(type->fnames[i]));
			else
				sprintf(bf, "       ret->fields.push_back(pair<char*, objrepr_t*>(\"%s\", %s(&obj->%s)));\n",type->fnames[i], \
					gen_func_by_type(curfield->name, NULL), type->fnames[i]);
			LINE(bf);
			}
		}

		LINE("  return ret;\n");
		LINE("  }\n\n");

		int len = 0;
		for (int i = 0; i<str.size(); i++)
			len += strlen(str[i]);
		ret->first = new char [len];
		ret->second = len;
		strcpy(ret->first, "");

		for (int i = 0; i<str.size(); i++)
		{
			strcat(ret->first, str[i]);
			delete [] str[i];
		}

		return ret;
	}


// vec neg
pair<char*, int>* repr_gen::neg_vec_func(type_t *type)
	{
		// if vector
		char bf[255];
		pair<char*, int>* ret = new pair<char*, int>;
		vector<char*> str;


		sprintf(bf, "   static %s* %s(objrepr_t *repr)\n\t{\n", type->name, neg_func_by_type(type->name, NULL));
		LINE(bf);

		type_t *curfield;

		sprintf(bf, "       %s* ret = new %s;\n", type->name, type->name);
		LINE(bf);
		//sprintf(bf, "	objrepr_t* elem;\n");
		//LINE(bf);

		sprintf(bf, "       for (int i = 0; i<repr->fields.size(); i++) \n\t\t{ \n");
		LINE(bf);

		type_t *invectype = type->gettype(nop(inside_vec(type->name)));

		// vector <int>
	/*	if (invectype->isbasetype())
		{
			if (!strcmp(inside_vec(type->name), "int"))
				{
				sprintf(bf,"	int ibf;\n");
				LINE(bf);
				sprintf(bf,"	sscanf(repr->at(i), \"%s\", &ibf);\n", "%d");
				}
				else if (!strcmp(inside_vec(type->name), "float"))
				{
				sprintf(bf,"	float ibf;\n");
				LINE(bf);
				sprintf(bf,"	sscanf(repr->at(i), \"%s\", &ibf);\n", "%f");
				}
				else if (!strcmp(inside_vec(type->name), "char") || !strcmp(inside_vec(type->name), "char*") \
					|| !strcmp(inside_vec(type->name), "char *"))
				{
				sprintf(bf,"	char *ibf = new char [255];\n");
				LINE(bf);
				sprintf(bf,"	strcpy(ibf, repr->at(i));\n");
				}
			LINE(bf);
			sprintf(bf, "	ret->insert(ibf);\n");
			LINE(bf);
		}
		// vector <class_t>
		else*/
		{
			if (type->ifp(invectype->name) || !strcmp(invectype->name, "char"))
			sprintf(bf, "       ret->push_back(%s(repr->fields.at(i).second));\n", \
					neg_func_by_type(invectype->name, NULL));
			else
				sprintf(bf, "       ret->push_back(*%s(repr->fields.at(i).second));\n", \
					neg_func_by_type(invectype->name, NULL));
			LINE(bf);
		}
		sprintf(bf, "       }\n");
		LINE(bf);


		LINE("  return ret;\n");
		LINE("  }\n\n");

		int len = 0;
		for (int i = 0; i<str.size(); i++)
			len += strlen(str[i]);
		ret->first = new char [len];
		ret->second = len;
		strcpy(ret->first, "");

		for (int i = 0; i<str.size(); i++)
		{
			strcat(ret->first, str[i]);
			delete [] str[i];
		}

		return ret;

	}


	// if not vector
pair<char*, int>* repr_gen::neg_func(type_t *type)
	{
		pair<char*, int>* ret = new pair<char*, int>;
		vector<char*> str;


	//	str.push_back(newline("HEIL\n"));  eqv LINE("HEIL\n")

		if (!strncmp(type->name, "vector<", 6))
		{
			//add_to_vec(&str,
			return	neg_vec_func(type);
			//goto GOTO_label_return;
		}

		char bf[255];
		sprintf(bf, "   static %s* %s(objrepr_t* repr)\n    {\n", type->name, neg_func_by_type(type->name, NULL));
		LINE(bf);


		type_t *curfield;

		sprintf(bf, "       %s* ret = new %s;\n", type->name, type->name);
		LINE(bf);
		//sprintf(bf, "	objrepr_t* curf;\n");
		//LINE(bf);
		for (int i = 0; i<type->fields.size(); i++)
		{
			curfield = type->gettype(type->fields[i]);
			// basetype
		/*	if (curfield->isbasetype())
			{
				if (!strcmp(curfield->name, "int"))
				{
				sprintf(bf,"	sscanf(repr->getfield(\"%s\"), \"%s\", &ret->%s);\n",\
					type->fnames[i], "%d", type->fnames[i]);
				}
				else if (!strcmp(curfield->name, "float"))
				{
				sprintf(bf,"	sscanf(repr->getfield(\"%s\"), \"%s\", &ret->%s);\n",\
					type->fnames[i], "%f", type->fnames[i]);
				}
				else if (!strcmp(curfield->name, "char") || !strcmp(curfield->name, "char*") \
					|| !strcmp(curfield->name, "char *"))
				{
				sprintf(bf,"	ret->%s = new char [255];\n",\
					type->fnames[i]);
				LINE(bf);
				sprintf(bf,"	strcpy(ret->%s, repr->getfield(\"%s\"));\n",\
					type->fnames[i], type->fnames[i]);
				}
			LINE(bf);
			//sprintf(bf, "	ret->fields.insert(pair<char*, objrepr_t*>(\"%s\", curfield));\n",type->fnames[i]);
			//LINE(bf);
			}
			// not basetype
			else*/
			{
			if (type->ifp(curfield->name) || isp(type->fnames[i]))
			sprintf(bf, "       ret->%s = %s(repr->getfield(\"%s\"));\n", nop(type->fnames[i]), \
					neg_func_by_type(nop(curfield->name), NULL), nop(type->fnames[i]));
			else
				sprintf(bf, "       ret->%s = *%s(repr->getfield(\"%s\"));\n", nop(type->fnames[i]), \
					neg_func_by_type(nop(curfield->name), NULL), nop(type->fnames[i]));
			LINE(bf);
			}
		}

		LINE("  return ret;\n");
		LINE("  }\n\n");

		int len = 0;
		for (int i = 0; i<str.size(); i++)
			len += strlen(str[i]);
		ret->first = new char [len];
		ret->second = len;
		strcpy(ret->first, "");

		for (int i = 0; i<str.size(); i++)
		{
			strcat(ret->first, str[i]);
		//	delete [] str[i];
		}

		return ret;
	}
