/**
*	@file	hcomms.h
*	@author	Artur Gilmutdinov <arturg000@gmail.com>
*	@brief	Contains hcomm_srv_t naming server for distributed objects.
*/


#pragma once
#include "common.h"
#include "hsock.h"
//#include "ereport.h"
#include "sendchannel.h"

#include "../../../hlogger/hlogger.h"

using namespace std;

class obj_rec_t
{
	public:
	string name;
	string pass;							// delete
	//send_channel_t *hb_ch;				// heartbeat channel

	obj_rec_t(string name, string pass)
	{
		this->name = name;
		this->pass = pass;
	}

	~obj_rec_t()
	{
            name.clear();
            pass.clear();
	}
};

/*
*	@class	node_t
*	@brief	Meta information about nodes that handle distributed objects.
*/
class node_t
{
	public:
	string ip;				// ip
	int port;					// node port
	string name;				// node name
	int status;
	map<string, obj_rec_t*> object;	// objects on node
        
        ~node_t()
        {
            /*
            vector<obj_rec_t*>::iterator it = object.begin();
            while (it!=object.end())
            {
                delete *it;
                object.erase(it);
            }
             * */
        }
        
//	bool check();
};

class portrange_t
{
	public:
	int begin;
	int end;
        int cur;
};

void  *hcomms_thread_fnc(void *a);
void *check_hb_fnc(void *a);

/*
*	@class	hcomm_srv_t
*	@brief	Naming server for objects.
*/
class hcomm_srv_t
{
	map<string,node_t*>  node;	// nodes
	portrange_t cliports;
	int srvport;
	hsock_t *sock;
	pthread_t srv_th;		// listen thread id
	pthread_t hb_th;		// heartbeat thread id
        
        
        
	//ereport_t *report;		// error report

//	Sends object's ip to clients. Called by serve.
	void serve_ifs(send_channel_t *ch);

//	Add node name and it's ip to table.
	void addnode(send_channel_t *ch, const char *ip);

//	Add record about object to table.
	void addobj(send_channel_t *ch);

	public:

        Hlogger *log;
            
	hcomm_srv_t(Hlogger *log)
	{
		sock = new hsock_t;
                this->log = log;
//		report = new ereport_t;
	}
        
        ~hcomm_srv_t()
        {
            /*
            vector<node_t*> ::iterator it = node.begin();
            while (it!=node.end())
            {
                delete *it;
                node.erase(it);
            }
             * */
        }

	hsock_t *get_sock()
	{
		return sock;
	}

	int get_srv_port()
	{
		return srvport;
	}

/*	ereport_t *getrep()
	{
		return report;
	}*/

//	Start listening clients.
	void start_server(int srvport, int cb, int ce)
	{
		this->srvport = srvport;
		cliports.begin = cliports.cur = cb;
		cliports.end = ce;
		pthread_create(&srv_th, NULL, hcomms_thread_fnc, this);
		//pthread_create(&hb_th, NULL, check_hb_fnc, this);
	}

        void join()
        {
            pthread_join(srv_th, NULL);
        }
        
	void kill_server()
	{
            log->log(EVENT0, "hcomm_srv_t::closing NS server ");
            sock->close_server(srvport);
            pthread_kill(srv_th, 0);
	}

    // called by hsock. Serve comm clients.
	void serve(pthread_t *th, int *socket, const char *ip, const int port)
	{
            send_channel_t *ch = new send_channel_t(socket);
            try
            {
		
		string command;
		command = ch->crecv();
                
                log->log(EVENT0, "hcomm_srv_t::new connection " + command);

		if (command == "ifs")
                {
			serve_ifs(ch);
                }
		else if (command == "addnode")
                {
			addnode(ch, ip);
                }
		else if (command == "addobj")
                {
                        log->log(EVENT0, "hcomm_srv_t::call addobj");
			addobj(ch);
                }

                //ch->close();
                delete ch;
            } 
            catch (string *s)
            {
                log->log(ERROR, string("serve EXCEPTION ") + *s );
                //ch->close();
                delete ch;
            }
            pthread_detach(*th);
	}
};
