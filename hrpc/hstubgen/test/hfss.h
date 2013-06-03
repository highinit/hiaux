#pragma once
#include <vector>
#include "string.h"
#include <iostream>
using namespace std;

#define STATEFILE "serverstate"
#define SERVEROBJ "hfsserver"

class bar
{
	public:
	int k;
};

class foo
{
	public:
	vector<bar> i;
	bar *b;
	float f;
	string c;
};

class fsnode_t
{
	public:
	string name;
	vector<string> files;
	void add(char *a) { files.push_back(a); }
};

class hfiles_t
{
	public:
	int crc;
	string name;
	vector<fsnode_t*> node;
	
	hfiles_t(char *nm, fsnode_t* nd, int crcc) { add(nm, nd, crcc); }
	void add(char *nm, fsnode_t* nd, int crcc) { strcpy(name, nm); node.push_back(nd); crcc = crc; }
};


class read_ret_t
{
	public:
	vector<bar> a;
	int crc;
};


class hfss_t
{
	public:
	vector<fsnode_t*> node;
	vector<hfiles_t*> file;
	
	
	void savestate();
	void loadstate();
	
	int fileid(char *name)
	{
	for (int i = 0; i<file.size(); i++)
	if (!strcmp(name, file[i]->name))
	return i;	
	}
	
	int nodeid(char *name)
	{
	for (int i = 0; i<node.size(); i++)
	if (!strcmp(name, node[i]->name))
	return i;	
	}
	
	// remote callable
	//#remote
	string caddnode(string noden, vector<int> files)
	{
            vector<hfiles_t> *files;
		cout << "adding fs node "<< noden;
		for (int i = 0; i<node.size(); i++)
		if (!strcmp(noden, node[i]->name))
			return "Node already exists.";
		fsnode_t *t = new fsnode_t;
		node.push_back(t);
		strcpy(t->name, noden);
		for (int i = 0; i<files->size(); i++)
		{
			t->add((*files)[i]);
			hfiles_t *f = new hfiles_t((*files)[i], t, 0);
			file.push_back(f);
		}
		return "OK";
	}
	
	//remote
	char* cadd(char *nodename, char *filename, int crc)
	{
		for (int i = 0; i<file.size(); i++)
		if (!strcmp(filename, file[i]->name))
		{
			return "exist\n";
		}
		
		for (int i = 0; i<node.size(); i++)
		if (!strcmp(nodename, node[i]->name))
		{
			// block node and files vector
			node[i]->add(filename);
			hfiles_t *f = new hfiles_t(filename, node[i], crc);
			file.push_back(f);
			cout << "-----------";		
			return "ok";
		}
	return "nonode";	
	}
	//remote return data storage name
	read_ret_t *cread(char *name, char *nodename)
	{
		read_ret_t r;
		int n = fileid(name);
		for (int i = 0; i<file[n]->node.size(); i++)
		{
//			r.nodes.push_back(file[n]->node[i]->name);	
		}
		file[n]->node.push_back(node[nodeid(nodename)]);
		r.crc = 0; 
		return &r;
	}
	//remote
	void crewrite(char *node, char *name)
	{
		// change table
		// send updates through pfor	
	}
	// filename
	void del(char *name);
	
};
