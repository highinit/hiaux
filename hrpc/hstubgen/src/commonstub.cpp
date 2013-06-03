
#include "incl.h"
#include "commonstub.h"

void puttabs(const int i)
{
	for (int j = 0; j<i; j++)
	cout << "	";
}

bool instring(const char *what,const char *wher)
{
	bool ok = 0;
	if (strlen(wher)<strlen(what)) return 0;

	int i = 0;
	do
	{
	ok = 1;
	for (int j = 0; j<strlen(what); j++)
	if (what[j]!=wher[i+j])
		{
		ok = 0;
		break;
		}
	if (ok) return 1;
	i++;
	} while (i+strlen(what)<=strlen(wher));
return 0;
}

bool firststr(const char *what, const char *wheres)
{
	if (strlen(what)>strlen(wheres)) return 0;
	for (int i = 0; i<strlen(what); i++)
		if (what[i]!=wheres[i]) return 0;
	return 1;
}

bool ischar(char *bf)
{
	if (bf[0]=='c')
	if (bf[1]=='h')
	if (bf[2]=='a')
	if (bf[3]=='r')
//	if (bf[4]=="*" || bf[4]==" ")
		return 1;
	return 0;
}

int firsts(char t, char *bf)
{
	for (int i = 0; i<strlen(bf); i++)
	if (bf[i]==t) return i;
return -1;
}


bool methodthere(char *shit, int *counter)
{
	if (instring("{", shit)) (*counter)++;
	if (instring("}", shit)) (*counter)--;

	bool op = 0, cl = 0;
	for (int i = 0; i<strlen(shit); i++)
	{
	if (shit[i]=='(') op = 1;
	if (shit[i]==')') cl = 1;
	}
	return op && cl;
}

bool fieldthere(char *shit, int *counter)
{
	if (instring("{", shit)) (*counter)++;
	if (instring("}", shit)) (*counter)--;

	bool op = 0, cl = 0;
	for (int i = 0; i<strlen(shit); i++)
	{
	if (shit[i]=='(') op = 1;
	if (shit[i]==')') cl = 1;
	}
	return !(op || cl) && instring(";", shit);
}

// char *a, char* a
void movestar(char *shit)
{
	for (int i = 0; i<strlen(shit)-1; i++)
	if (shit[i]==' ')
	if (shit[i+1]=='*')
	{
		shit[i] = '*';
		shit[i+1] = ' ';
	}
}

void backstar(char *shit)
{
	for (int i = 0; i<strlen(shit)-1; i++)
	if (shit[i]=='*')
	if (shit[i+1]==' ')
	{
		shit[i] = ' ';
		shit[i+1] = '*';
	}
}

void removespaces(char *shit)
{
	for (int i = 0; i<strlen(shit)-1; i++)
	if (shit[i]==' ')
	if (shit[i+1]==' ')
	memcpy(&shit[i], &shit[i+1], sizeof(char)*(strlen(shit)-1-i));
}

const char *nop(const char *bf)
{
	if (bf[0]!='*' && bf[strlen(bf)-1]!='*') return bf;
	char bf2[255];
	char *bf1 = new char [255];
	strcpy(bf2, bf);
	if (bf[0]=='*')
	bf2[0] =  ' ';
	else bf2[strlen(bf)-1] = '\0';


	sscanf(bf2, "%s", bf1);

	return bf1;
}

bool isp(char *bf)
{
	if ((bf[strlen(bf)-1]=='*')||(bf[0]=='*')) return 1;
	return 0;
}

void firstspace(char *shit)
{
	while(shit[0]==' ')
	memcpy(&shit[0], &shit[1], sizeof(char)*(strlen(shit)-1));
}

void lastspace(char *shit)
{
	int i = strlen(shit)-1;
	while (shit[i]==' ')
	{
	shit[i] = '\0';
	i--;
	}

}

void tospace(char t, char *bf)
{
	for (int i = 0; i<strlen(bf); i++)
	if (bf[i]==t) bf[i] = ' ' ;
}

void next(char *bf, char *to)
{

	sscanf(bf, "%s" , to);
	memcpy(bf, &bf[strlen(to)+1], sizeof(char)*(strlen(bf)-strlen(to)));
//	bf[strlen(bf)-strlen(to)] = '\0';
}

// -> alternative
char *addptolast(char *l, char *n)
{
	char *bf = new char [255];
	strcpy(bf, "(*");
	if(strlen(l)!=0)
	{
	strcat(bf, l);
	if (n[0]!='[') strcat(bf,".");
	}
	n[0] = ' ';
	strcat(bf, n);
	strcat(bf, ")");
	return bf;
}

// . implementation
char *addtolast(const char *l, const char *n)
{

	char *bf = new char [255];
	strcpy(bf, l);
//	removespaces(bf);
	if (strlen(l)!=0) if (n[0]!='[')	strcat(bf, ".");
	strcat(bf, n);
	return bf;
}

	int findfirst(char *a, char t)
	{
	for (int i = 0; i<strlen(a); i++)
		if (a[i]==t) return i;
	return -1;
	}

	char* inside_vec(const char *type)
	{
		if (instring("vector ",type) || instring("vector<",type))
		{
			char bf[255];
			strcpy(bf, type);
			int l = findfirst(bf, '<');
			int r = findfirst(bf, '>');
			while(bf[l+1]==' ') l++;
			while(bf[r-1]==' ') r--;

			char *ret = new char[255];
			strcpy(ret, "");
			for (int i = l+1; i<r; i++)
				ret[i-l-1] = bf[i];
			ret[r-l-1] = '\0';
			return ret;
		}
	return NULL;
}



bool typeqv(const char *a, const  char *b)
{
	char s1[50];
	char s2[50];

	if (inside_vec(a)!=inside_vec(b)) return 0;
	if (inside_vec(a)!=NULL) return 1;
	sscanf(a, "%s", s1);
	sscanf(b, "%s", s2);
	if (!strcmp(s1,s2)) return 1;

	return 0;
}

void add_to_str(pair<char*, int>* a, pair<char*, int>* b)
{
	int newsize = a->second + b->second;
	char *bf = new char [newsize+1];
	for (int i = 0; i<newsize; i++)
	{
		if (i<a->second) bf[i] = a->first[i];
		else bf[i] = b->first[i-a->second];
	}
	pair<char*, int>* ret = new pair<char*, int>(bf, newsize);
	delete [] a->first;
	a->first = ret->first;
	a->second = ret->second;
}

// test it
void add_str_to_str(pair<char*, int>* a, const char* b)
{
	int newsize = a->second+strlen(b);
	char *bf = new char [newsize];
	for (int i = 0; i<newsize; i++)
	{
		if (i<a->second) bf[i] = a->first[i];
		else bf[i] = b[i-a->second];
	}

	pair<char*, int>* ret = new pair<char*, int>(bf, newsize);
}


char *gen_func_by_type(const char *type, const char* fgclass)
	{
		char *bf = new char [255];
		if (!strncmp(type, "vector<", 6))
		{
			char bf1[255];
			char bf2[255];
			char bf3[255];
			strcpy(bf1, type);
			for (int i =0; i<strlen(bf1); i++)
			if (bf1[i]=='<' || bf1[i]=='>' || bf1[i]=='*' ) bf1[i] = ' ';
			sscanf(bf1, "%s %s", bf2, bf3);
			if (fgclass==NULL)
                sprintf(bf, "vec_%s_gen", bf3);
			else
                sprintf(bf, "%s::vec_%s_gen", fgclass, bf3);
			return bf;
		}
		if (fgclass==NULL)
            sprintf(bf, "%s_gen", type);
		else
            sprintf(bf, "%s::%s_gen", fgclass, type);
		return bf;
	}

char *neg_func_by_type(const char *type, const char* fgclass)
	{
		char *bf = new char [255];
		if (!strncmp(type, "vector<", 6))
		{
			char bf1[255];
			char bf2[255];
			char bf3[255];
			strcpy(bf1, type);

			for (int i =0; i<strlen(bf1); i++)
			if (bf1[i]=='<' || bf1[i]=='>' || bf1[i]=='*' ) bf1[i] = ' ';

			sscanf(bf1, "%s %s", bf2, bf3);
			if (fgclass==NULL)
                sprintf(bf, "vec_%s_neg", bf3);
			else
                sprintf(bf, "%s::vec_%s_neg", fgclass, bf3);
			return bf;
		}
		if (fgclass==NULL)
            sprintf(bf, "%s_neg", type);
		else
            sprintf(bf, "%s::%s_neg", fgclass, type);
		return bf;
	}
