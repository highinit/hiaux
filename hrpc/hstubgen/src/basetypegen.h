
char *basetype_gen_funcs =  "\
	static objrepr_t *int_gen(int *a)\n\
	{\n\
		objrepr_t *repr = new objrepr_t;\n\
                char bf[15];\n\
                strcpy(bf, \"\");\n\
		sprintf(bf, \"%d\", *a);\n\
                repr->value = bf;\n\
		repr->type = \"int\";\n\
		return repr;\n\
	}\n\
\n\
	static objrepr_t *float_gen(float *a)\n\
	{\n\
		objrepr_t *repr = new objrepr_t;\n\
		char bf[15];\n\
                strcpy(bf, \"\");\n\
		sprintf(bf, \"%f\", *a);\n\
                repr->value = bf;\n\
		repr->type = \"float\";\n\
		return repr;\n\
	}\n\
\n\
	static objrepr_t *char_gen(string *a)\n\
	{\n\
		objrepr_t *repr = new objrepr_t;\n\
		repr->value = *a;\n\
		repr->type = \"string\";\n\
		return repr;\n\
	}\n\
\n\
	static int *int_neg(objrepr_t *repr)\n\
	{\n\
		int *r = new int;\n\
		sscanf(repr->value.c_str(), \"%d\", r);\n\
		delete repr;\n\
		return r;\n\
	}\n\
\n\
	static float *float_neg(objrepr_t *repr)\n\
	{\n\
		float *r = new float;\n\
		sscanf(repr->value.c_str(), \"%f\", r);\n\
		delete repr;\n\
		return r;\n\
	}\n\
\n\
	static string *char_neg(objrepr_t *repr)\n\
	{\n\
		string *r = new string;\n\
		*r = repr->value;\n\
		delete repr;\n\
		return r;\n\
	}\n\
\n";
