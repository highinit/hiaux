#pragma once
#include "common.h"
#include "hqueue.h"

#define OK 0
#define FAULT 1

class send_channel_t
{
    
        int *sock;
        bool otherside_close_confirm_waiting;
        bool thisside_waiting_close;
        
	public:
            
        static hsock_t hsock;


        send_channel_t();
        ~send_channel_t();

        send_channel_t(int *sock);

       // void recv_hb();
       // void send_hb();

        int csend(string str);
        string crecv();
	
        //int check_fault();
        void connect(string ip, int port);
        void close();
        
        static void close_all_servers()
        {
            hsock.close_servers();
        }
        
        static void close_server(int portno)
        {
            hsock.close_server(portno);
        }
};

