/*
*	@file	common.h
*	@author	Artur Gilmutdinov <arturg000@gmail.com>
*	@brief	Some useful functions.
*/

#pragma once
#include <map>
#include "semaphore.h"
#include <vector>
#include <cstdio>
#include "hsock.h"
//#include "objrepr.h"
#include <queue>
//#include "sendchannel.h"
using namespace std;

namespace hcommon
{

class log_t
{
public:
static bool opened;
static char fname[255];
static bool screen;
static sem_t sem;

	static void openlog(char *fn)
	{
		sem_init(&sem, 0, 1);
		opened = 1;
		strcpy(fname, fn);
		FILE *f = fopen(log_t::fname, "wt");
		fclose(f);
	}

	static void openlog()
	{
		screen = 1;
		opened = 1;
	}

	static void log(const char *text)
	{
		sem_wait(&sem);
		if (!opened) return;
		if (screen) { cout << text; return; }
		FILE *f = fopen(fname, "a+");
		fprintf(f, "%s\n", text);
		fclose(f);
		sem_post(&sem);
	}
};

};

//using namespace std;

template <class type>//, class ret_t>
struct arg_t
{
	type *obj;
	void (type::*func)(void*);
//	ret_t *ret;
	void *arg;
};

template <class type>//, class ret_t>
void *thread_func(void *a)
{
	arg_t<type>  *b =	(arg_t<type> *)a;
	//b->ret = *
	((b->obj)->*(b->func))(b->arg);
	return 0;
}

/*
class pfor_t
{
	vector<pthread_t> th;
	public:
	template <class type, class ret>
	vector<ret> pfor(int i, type *t, ret(type::*f)(int, void *), void *a)
	{
		int thr;
		th.push_back(thr);
		arg_t<type> arg;
		arg.f = f;
		arg.n = i;
		arg.t = t;
		arg.a = a;
		pthread_create(&th[th.size()], NULL, tfunc<type,ret>, &arg);
		pthread_join(th[th.size()] ,0);
	}
	void pbreak()
	{
		for (int i = 0; i<th.size(); i++)
			pthread_cancel(th[i]);
	}
};
*/
class hint
{
	int value;
	public:
	virtual hint operator=(int b)
	{
		hint p;
		p.value = b;
		return p;
	}
	virtual int operator=(hint b)
	{
		int p;
		p = b.value;
		return p;
	}
	virtual char* operator<<(hint *a)
	{
		char *bf = new char [255];
		sprintf(bf, "%d\n", a->value);
		return bf;
	}
	virtual hint* operator>>(char *a)
	{
		hint *b = new hint;
		sscanf(a, "%d", &b->value);
		return b;
	}
};

class hfloat
{
	float value;
	public:
	virtual hfloat operator=(int b)
	{
		hfloat p;
		p.value = b;
		return p;
	}
	virtual float operator=(hfloat b)
	{
		float p;
		p = b.value;
		return p;
	}
	virtual char* operator<<(hfloat *a)
	{
	char *bf = new char [255];
	sprintf(bf, "%f\n", a->value);
	return bf;
	}
	virtual hfloat* operator>>(char *a)
	{
	hfloat *b = new hfloat;
	sscanf(a, "%f", &b->value);
	return b;
	}
};


char *inttostr(int a);
int strtoint(char *a);
bool strsrav(const char *a, const char *b);




void *send_ch_func(void *a);



char *newline(const char *str);
char *newline_clear(char *str);
vector<char*> splitbydots(const char *path);

template <class Foo>
static bool deleteAll( Foo * theElement ) { delete theElement; return true; }
