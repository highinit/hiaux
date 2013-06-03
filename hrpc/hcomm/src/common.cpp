
#include "../../hcomm/include/common.h"

/*
char log_t::fname[255];
bool log_t::opened = 0;
bool log_t::screen = 0;
sem_t log_t::sem;

#define log log_t::log
*/
/*
char *inttostr(int a)
{
	char bf[255];
	sprintf(bf,"%d", a);
	return bf;
}*/

int strtoint(char *a)
{
	int b;
	sscanf(a, "%d", &b);
	return b;
}

bool strsrav(const char *a, const char *b)
{
//	log("------");
//	log(a);
//	log(b);
//	log("------");
	int i = 0;
	while (a[i]!='\n' && b[i]!='\n' && a[i]!='\0' && b[i]!='\0')
	{
		if (a[i]!=b[i]) return 0;
		i++;
	}

//	if (a[i]=='\0' ||  a[i]=='\0')
//	if (b[i]=='\0' ||  b[i]=='\0') return 1;
	return 1;
}


char *newline(const char *str)
{
	char *bf = new char [255];
	strcpy(bf, str);
return bf;
}

char *newline_clear(char *str)
{
	char *bf = new char [255];
	strcpy(bf, str);
	strcpy(str, "");
return bf;
}

vector<char*> splitbydots(const char *path)
{
	vector<char*> subobjs;
	char bf[255];
	strcpy(bf,"");
	int l = 0;
	for (int i = 0; i<strlen(path); i++)
		{
			if (path[i]=='.')
			{
				bf[l] = '\0';
				subobjs.push_back(newline_clear(bf));
				l = 0;
				i++;
			}
			bf[l] = path[i];
			l++;
		}
	bf[l] = '\0';
	subobjs.push_back(newline_clear(bf));

	return subobjs;
}

void *send_ch_func(void *a)
{
//	send_channel_t *ch = (send_channel_t*)a;
//	ch->recv_thread();
	return 0;
}
