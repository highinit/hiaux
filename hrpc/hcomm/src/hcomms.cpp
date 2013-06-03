/*
*	@file	hcomms.cpp
*	@author Artur Gilmutdinov <arturg000@gmail.com>
*/

#include "../include/hcomms.h"

// thread client serve
void  *hcomms_thread_fnc(void *a)
	{
                hcomm_srv_t *b = (hcomm_srv_t*)a;
                try
                {
                        b->get_sock()->server<hcomm_srv_t>(b, b->get_srv_port());
                } catch (string* s)
                {
                        b->log->log(ERROR, "EXCEPTION hcomm_srv_t:: " + *s);
                        exit(0);
                }
		return 0;
	}

// thread check heartbeats
void *check_hb_fnc(void *a)
	{
		hcomm_srv_t *ns = (hcomm_srv_t*)a;
		//ns->start_hb();
		return 0;
	}

// serve ifs
void hcomm_srv_t::serve_ifs(send_channel_t *ch)
	{
		string obj_name = ch->crecv();
                log->log(EVENT0 , "hcomm_srv_t::ifs " + obj_name);
		
                node_t *target_node = NULL;
                
                
                map<string, node_t*>::iterator node_it = node.begin();
		while (node_it!=node.end())
		{
                    map<string,obj_rec_t*>::iterator obj_it = node_it->second->object.begin();
                    while (obj_it!=node_it->second->object.end())
                    {
//			log(node[i]->object[j]->name);
                            if (obj_name == obj_it->first)
                            {
                                    target_node = node_it->second;
                                    break;
                            }
                            obj_it++;
                    }
                    if (target_node!=NULL) break;
                    node_it++;
		}
		if (target_node==NULL)
		{
                        ch->csend("error no such object");
                        ch->csend("error no such object");
                        //ch->close();
			return;
		}
                
                
		ch->csend(target_node->ip);
                
                char bf[255];
		sprintf(bf, "%d\n", target_node->port);
		ch->csend(string(bf));

		//ch->close();

	}
// add node
void hcomm_srv_t::addnode(send_channel_t *ch, const char *ip)
{
        char port_s[255];
	string name = ch->crecv();

        log->log(EVENT0 , "hcomm_srv_t::addnode " + name);
        
	// Create new node.
	node_t *n = new node_t;
	
        map<string, node_t*>::iterator node_it = node.find(name);
        if (node_it==node.end())
        {
            n->name =  name;
            n->ip = ip;
            node.insert(pair<string, node_t*>(name, n));
        }
        else
        {
            delete node_it->second;
            n->name =  name;
            n->ip = ip;
            node_it->second = n;
        }
        
        node_it = node.begin();
	int offport = 0;
	while (node_it!=node.end())
        {
		if (strsrav(node_it->second->ip.c_str(),ip)) offport++;     
                node_it++;
        }
	if (cliports.begin+offport>cliports.end)
	{
                strcpy(port_s, "not enough ports");
	}
	else
	{
		sprintf(port_s, "%d", cliports.begin+offport);
		n->port = cliports.begin+offport;
	}

        cliports.begin++;
	ch->csend(string(port_s));
        log->log(EVENT0 , "hcomm_srv_t::addnode OK");
}

// add obj
void hcomm_srv_t::addobj(send_channel_t *ch)
{
        log->log(EVENT0 , "hcomm_srv_t::addobj start");
	string node_name = ch->crecv();
	string obj_name = ch->crecv();

        log->log(EVENT0 , "hcomm_srv_t::addobj " + obj_name + " on " + node_name);
        
        bool ok = 0;
        
        if (obj_name!="")
        {
                map<string, node_t*>::iterator node_it = node.find(node_name);
                if (node_it!=node.end())
                {
                    map<string,obj_rec_t*>::iterator obj_it = node_it->second->object.find(obj_name);
                    if (obj_it!=node_it->second->object.end())
                    {
                        log->log(EVENT0 , "hcomm_srv_t::addobj object already exist OK");
                        ch->csend("ok");
                        ok = 1;
                    }
                    else
                    {
			obj_rec_t* obj = new obj_rec_t(obj_name, "");
			node_it->second->object.insert(pair<string, obj_rec_t*>(obj_name, obj));
                        log->log(EVENT0 , "hcomm_srv_t::addobj OK");
			ch->csend("ok");
                        ok = 1;
                    }
                }
                else
                {
                    log->log(WARNING , "hcomm_srv_t::addobj " + node_name + " not found");
                    ch->csend("error");
                }

        }
        
        if (!ok)
        {
                ch->csend("error");
        }
			
        log->log(EVENT0 , "hcomm_srv_t::addobj " + obj_name + " on " + node_name + " OK");
}
