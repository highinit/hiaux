#include "extract_class.h"

//vector<char*> stubgen_t::extracter::include;

// basetype id
int extracter_t::basetype(const char *bf)
{
	//printf("%s\n", bf);
	for (int i = 0; i<5; i++)
	if ((!strcmp(bf, (*type)[i]->name)))
	{
		//||(instring("vector<",bf)&&(instring(type[i]->name, bf))))
	//printf("%d ", i);
	return i;
	}
	return -1;
}


// extract type from file
void extracter_t::find_type_infile(const char *tname1)
{
		char *tname = new char [255];
		strcpy(tname, nop(tname1));
	//	printf("%s\n",tname);
		//cout << tname;
		type_t *t = new type_t(UNDEF, tname, type);
		FILE *f = fopen(fname, "rt");
		char *bf = new char [255];
		strcpy(bf,"");
		int counter = 1;
		//	printf("---%s %s\n", tname, bf);
		// class found
		lastspace(tname);
		while(1)
		{
			if (feof(f))
			{
				fclose(f);
				return;
			}

			fgets(bf, 255, f);
			if (instring("class",bf))
			{
		//	printf("%s %s\n",tname ,bf);
		//	printf("%d %d",strlen(tname) ,strlen(bf));
			if (instring(tname, bf))
				{
			//	printf("match!!!!!!!!");
				break;
				}
			}
		}

		if (feof(f))
		{
			fclose(f);
			return;
		}

		fgets(bf, 255, f);
		fgets(bf, 255, f);
		fgets(bf, 255, f);

		char *bf1;
		while(!instring("};", bf))
		{
			// get fields
			if (fieldthere(bf, &counter))
			if  (counter==1)
			{
				if (strlen(bf)==0) continue;
				// here field of tname
				bf1 = new char [255];
				firstspace(bf);
				movestar(bf);
				if (!instring("char*",bf)) backstar(bf);

				sscanf(bf, "%s %s", bf1, bf);
	//			cout  << bf1 << " ";
				char *fname = new char[255];
				tospace(';',bf);
				strcpy(fname, bf);
				t->addfield(bf1, fname);
			}
			fgets(bf, 255, f);
			if (feof(f))
			{
				fclose(f);
				return;
			}
		}
		fclose(f);
		//cout << t->fields.size() << endl;
		type->push_back(t);
		for (int i = 0; i<t->fields.size(); i++)
		{
			//	cout << t->fields[i] << " ";
				find_type(nop(t->fields[i]), t->fnames[i]);
		}
}

// create new type in extracter
void extracter_t::find_type(const char *tname1, const char *varname)
	{
		char *tname = new char [255];
		strcpy(tname, tname1);

		//cout << tname << endl;
		if (typexist(tname)) return;
		if (typexist(nop(tname))) return;
		char bf[255];
		char bf1[255];
		char bf2[255];
		if (instring("vector<", tname))
		{
			tospace('<', tname);
			tospace('>', tname);
			sscanf(tname,"%s %s", bf1, bf);
			type_t *t = new type_t(-1, tname1, type);
			t->vec = 1;
			t->basetype = basetype(nop(bf));
			t->addfield(bf, varname);
	//		cout << bf;
			type->push_back(t);
			//find_type(nop(bf), varname);
			find_type(nop(bf), varname);
			//return;
		}
		//else cout << bf << endl;
		else find_type_infile(nop(tname1));

	}

