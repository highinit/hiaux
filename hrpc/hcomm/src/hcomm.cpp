
/*
*	@file	hcomm.cpp
*	@author	Artur Gilmutdinov <arturg000@gmail.com>
*/

#include "../include/hcomm.h"
#include "../include/hcomms.h"

// serve remote ifs
void hcomm_t::serve_ifs(send_channel_t *ch)
{
	string obj_name = ch->crecv();

        logger->log(EVENT0, name+"::hcomm_t::serve_ifs:: opening serving ifs for local object "+obj_name);
        
	map<string, skel_t*>::iterator it = obj.find(obj_name);

	if (it==obj.end())
	{
		ch->csend("no such object on node");
		return;
	}

        ch->csend("ok");
        
        logger->log(EVENT0, name+"::hcomm_t::serve_ifs:: ifs opened");
	while (1)
	{       
		string command = ch->crecv();
		if (command == "") continue;
		if (command == "ifs stop")
		{
                        logger->log(EVENT0, name + "::hcomm_t::serve_ifs:: stopping ifs for " + obj_name);
                        return;
		}
#ifdef HRPC_LOG_CALLS
                logger->log(EVENT0, name + "::hcomm_t::serve_ifs:: calling method "+command + " for " + obj_name);
#endif                
                it->second->call(ch, command);               
                ch->csend("call finished");
#ifdef HRPC_LOG_CALLS
                logger->log(EVENT0, name + "::hcomm_t::serve_ifs:: call finished");         
#endif
	}
}

// serve input connections
void hcomm_t::serve(pthread_t *th, int *socket, char *ip, int port)
{
    try
    {
	send_channel_t *ch = new send_channel_t(socket);
	string command = ch->crecv();

	if (command=="ifs")
		serve_ifs(ch);
	else if (command == "upload")
	{
		uploadfile(*socket);
	}
        
        ch->close();
        delete ch;
        //pthread_detach(*th);
        //delete th;
        delete ip;
        delete socket;
    }
    catch (string *s)
    {
        logger->log(ERROR, string("EXCEPTION hcomm_t::serve ") + *s );
       // exit(0);
    }
}

void hcomm_t::connect()
{
	send_channel_t ch;
        ch.connect(srvip, srvport);
	ch.csend("addnode");
	ch.csend(name);
	
        string port_s = ch.crecv();
        sscanf(port_s.c_str(), "%d", &cliport);
        logger->log(EVENT0, name + "::hcomm_t::connected to " + srvip + " local ifs server on port " + port_s);   
}

// upload file
void hcomm_t::uploadfile(int socket)
{
	char bf[255];
	recv(socket, bf, 255, 0);
	if (pathsolver==NULL)
		throw "pathsolver undefined";
	FILE *f = fopen(pathsolver->pathsolve(bf), "r");
	fseek(f, 0L, SEEK_END);
	int size = ftell(f);
	fseek(f, 0L, SEEK_SET);

	sprintf(bf, "%d\n", size);
	send(socket, bf, 255, 0);

	while (size>255*sizeof(char))
	{
		fread(bf,sizeof(char)*255, 1 , f);
		send(socket, bf, 255, 0);
		size-=255*sizeof(char);
	}
	fread(bf,sizeof(char)*255, 1 , f);
	send(socket, bf, 255, 0);
	fclose(f);
}

// start local server thread
void *srv_fnc(void *a)
	{

		hcomm_t *b = (hcomm_t*) a;
		
		send_channel_t::hsock.server<hcomm_t>(b, b->cliport);

		return 0;
	}

// start server
void hcomm_t::start_server()
	{

		pthread_create(&srv_th, NULL, srv_fnc, this);
//		hsock_t s;
//		s.server<hcomm_t>(this, cliport);

	}

void hcomm_t::join()
{
	pthread_join(srv_th, NULL);
}

void hcomm_t::kill_server()
{
	pthread_kill(srv_th, 0);
}
