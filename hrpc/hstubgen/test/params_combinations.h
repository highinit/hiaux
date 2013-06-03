
//#include "common.h"

class bar1_t
{
public:
	int c;
};

class bar_t
{
public:
	bar1_t a;
	int b;
};

class bar2_t
{
public:
	bar1_t *c;
	vector<int> d;
};

class bar3_t
{
public:
	vector<int> *d;
};



class remote_class_t
{
public:

	//#remote
	char *foo()
	{

		return "foo test call!";
	}

	//remote
	int foo1(int a)
	{
		if (a==4) return 42;
		else return -1;
	}

	//remote
	int foo2(bar_t* bar)
	{
		if (bar->a.c+bar->b==42) return 1488;
		return 0;
	}

	//remote
	int* foo3(bar_t bar)
	{
		int *a = new int;
		*a = 1488;
		if (bar.a.c+bar.b==42) return a;
		*a = -1;
		return a;
	}

	//remote fails!!!!!
	int foo4(bar2_t* bar)
	{
		if (bar->c->c+bar->d[0]==42) return 1488;
		return -1;
	}

	//remote
	vector<char*> foo5()
	{
		vector<char*> a;
		char *bf = new char [255];
		strcpy(bf, "sieg");
		a.push_back(bf);
		return a;
	}

	//remote
	vector<char*>* get_posts(char *bywho, char *id)
	{
		vector<char*> *a = new vector<char*>;
		char *bf = new char [255];
		strcpy(bf, "sieg");
		a->push_back(bf);
		return a;
	}

	//remote
	bar3_t *foo7()
	{
		bar3_t *t = new bar3_t;
		t->d = new vector<int>;
		t->d->push_back(42);
		return t;
	}
};
