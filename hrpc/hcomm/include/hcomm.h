/*
*	@file	hcomm.h
*	@author	Artur Gilmutdinov <arturg000@gmail.com>
*	@brief	Contains distributed system's client hcomm_t.
*/

#pragma once

#include "common.h"
#include "hcomms.h"

#include "../../../hlogger/hlogger.h"

using namespace std;

inline void red(char *t, int l)
{
	for (int i = 0; i<l; i++) t[i] = ' ';
}

/*
*	@class	stubs_t
*	@brief	Abstract class for skeletons.
*/
class skel_t
{
	public:
	send_channel_t *ch;

	// Calls real object's methods.
	virtual char *call(send_channel_t *ch, string&) = 0;
};

/*
*	@class	stubc_t
*	@brief	Abstract class for client stubs.
*/
class stub_t
{

int socket;
public:
send_channel_t *ch;

	// Called by user. Save file to local filesystem from other node.
	int downloadfile(char* name, char* path)
	{
		char bf[255];

		strcpy(bf, "upload\n");
		send(socket, bf, 255, 0);
		strcpy(bf, name);
		strcat(bf,"\n");
		send(socket, bf, 255, 0);

		FILE * f = fopen(path, "w");
		int size;
		// Get node ip from node that is filesystem nameserver (not hcomms_t).
		recv(socket, bf, 255, 0);
		sscanf(bf, "%d", &size);

		while(size>255*sizeof(char))
		{
			recv(socket, bf, 255, 0);
			fwrite(bf,sizeof(char)*255, 1 , f);
			size-=255*sizeof(char);
		}
		recv(socket, bf, 255, 0);
		fwrite(bf,sizeof(char)*255, 1 , f);
	fclose(f);
	return 0;
	}
};

/*
*	@class	pathsolver_t
*	@brief	Abstract class for name resolving on filesystem's nameserver when client calls.
*/
class pathsolver_t
{
	public:
	virtual char *pathsolve(char*);
};

/*
*	@class	hcomm_t
*	@brief	Distributed system client. Share objects and invoce remote methods.
*/
class hcomm_t
{
	public:
	string srvip;
	int socket;
	int cliport, srvport;
	string name;
	pthread_t srv_th;
        
        Hlogger *logger;
	// Local remote object's stubs.
	map<string, skel_t*> obj;

	// Pathresolver.
	pathsolver_t *pathsolver;

	// Serve remote interface.
	void serve_ifs(send_channel_t *ch);
	//void serve_checkobj(int socket);

	public:

	hcomm_t(string srvip, int port, string nm, Hlogger *logger)
	{
                this->logger = logger;
                srvport = port;
		pathsolver = NULL;
		this->srvip = srvip;
		this->name = nm;
	}
        
        ~hcomm_t()
        {
            obj.clear();
        }
        
	// Called by hsock when client connects.
	void serve(pthread_t *th, int *socket, char *ip, int port);

	// Called by serve when client wants to upload something.
	void uploadfile(int socket);
	void set_pathsolver(pathsolver_t *p) { pathsolver = p; }

	// Share local object.
	template <class type, class stubt>
	void share_obj(type *data, string obj_name);
	// Open remote interface.
	template <class type>
	type* open_ifs(type *cstub, string obj_name);
	// start heartbeating
	void start_heartbeats(send_channel_t *ch);

	// Start listening connections.
	void start_server();
	// join
	void join();
	// kill
	void kill_server();
	// Connect to object naming server (hcomms_t).
	void connect();
};

// share obj
template <class type, class stubt>
void hcomm_t::share_obj(type *data, string obj_name)
{
        logger->log(EVENT0, name+"::hcomm_t::share_obj:: sharing object "+obj_name);
	
        logger->log(EVENT0, name+"::hcomm_t::share_obj:: registering object on nameserver");
        send_channel_t *ch = new send_channel_t();
        
        try
        {
                ch->connect(srvip, srvport);
        } catch (string *s)
        {
                logger->log(EVENT0,name+"::hcomm_t::share_obj::connecet " + *s);
                ch->close();
                delete ch;
                throw *s;
        }
        logger->log(EVENT0, name+"::hcomm_t::share_obj:: connecetd to NS"); 
       
	ch->csend("addobj");
	ch->csend(this->name);
	ch->csend(obj_name);
        
        
                
        string err = ch->crecv();
        logger->log(EVENT0, name+ " NS returned " + err);
        if (err== "ok")
        {
             logger->log(EVENT0, name+":: object registered on nameserver");
        }
        else
        {
             logger->log(ERROR, name+":: ERROR: registering object on nameserver"+err);
        }

	ch->close();
	delete ch;
        
        
        stubt *stub = new stubt(data);
        
	pair<string, skel_t*> o(obj_name, stub);
        obj.insert(o);
        
        logger->log(EVENT0, name+"::hcomm_t::share_obj OK");
}

// return ifs
template <class type>
type* hcomm_t::open_ifs(type *cstub, string obj_name)
{
        logger->log(EVENT0, name + "::hcomm_t::open_ifs:: start");
	type *cstub_loc;
	if (cstub==NULL)
	{
		cstub_loc = new type(this, name);
	}
	else cstub_loc = cstub;
                
        // resolve object location
	send_channel_t *ch = new send_channel_t();
        ch->connect(srvip, srvport);

	ch->csend("ifs");
	ch->csend(obj_name);
		
        string obj_ip = ch->crecv();
	string obj_port = ch->crecv();
        
        logger->log(EVENT0, name + "::hcomm_t::open_ifs:: got endpoint " + obj_ip + " " + obj_port);
        
        ch->close();
        
        // check errors
        /*
        if (obj_ip!='e')
        {
            LOG(name + "::hcomm_t::open_ifs:: ERROR " + obj_ip);
            delete cstub_loc;
            return NULL;
        }*/
                
        int obj_port_i;
        sscanf(obj_port.c_str(), "%d", &obj_port_i);
	
	// connect to node
	cstub_loc->ch = new send_channel_t();
        cstub_loc->ch->connect(obj_ip, obj_port_i);

	cstub_loc->ch->csend("ifs");
	cstub_loc->ch->csend(obj_name);
        
        string err = cstub_loc->ch->crecv();
        
        if (err!="ok")
        {
            logger->log(ERROR, name + ":: hcomm_t::open_ifs:: ERROR" + err);
            delete cstub_loc;
            return NULL;
        }
        logger->log(EVENT0, name + ":: hcomm_t::open_ifs:: ifs for remote object opened for " + obj_name);
                 
	return cstub_loc;
}
