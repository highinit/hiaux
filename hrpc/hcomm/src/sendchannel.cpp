#include "../include/sendchannel.h"
//#include "../include/sendchman.h"
//#include "../include/hlog.h" 
//#define LOG(str) hlog_log(str, "HdividerWatcher.log")

hsock_t send_channel_t::hsock;
/*
int send_channel_t::csend(char *str)
{
        char *packet = new char[255];
        strcpy(packet,"");
        strcpy(packet, str);
        if (send(*sock, packet, 255, 0)<=0)
        {
			//state = FAULT;
        }
        delete [] packet;
        return 0;
}*/

int send_channel_t::csend(string str)
{
        //str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
        //str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        //if (str.length()==0) return 1;
    
         
    
        char packet_size_str[10];
        
        for (int i = 0; i<10; i++)
            packet_size_str[i] = 0;
        
        if (str.length()<999999)
        {
            sprintf(packet_size_str, "%d", (int)str.length()+2);
            packet_size_str[8] = '\r';
            packet_size_str[9] = '\n';
           // strcat(packet_size_str, "\r\n");
        }
        else
        {
            throw new string("send_channel_t::csend too big packet" + str);
        }
        
        int err = send(*sock, packet_size_str, 10, 0);
        if (err==-1)
        {
            throw new string ("send_channel_t::csend failed to send packet_size_str"); 
        }
        else if (err!=10)
        {
            throw new string ("send_channel_t::csend not 5 bytes"); 
        }
        
        if (str.length()==0) return 0;
        
        char *packet = new char[str.length()+2];
        
        for (int i = 0; i<str.length()+2; i++)
            packet[i] = 0;
 
        
        strcpy(packet,"");
        strcpy(packet, str.c_str());
        //strcat(packet, "\r\n");
        packet [str.length()] = '\r';
        packet [str.length()+1] = '\n';
        
        err = send(*sock, packet, str.length()+2, 0);
        if (err==-1)
        {
            throw new string ("send_channel_t::csend failed to send packet_size_str"); 
        }
        else if (err!=str.length()+2)
        {
            throw new string ("send_channel_t::csend not str.length()+2 bytes"); 
        }
        
        delete [] packet;
        return 0;
}


string send_channel_t::crecv()
{
        int packet_size = 255;
    
        char packet_size_str[10];
        int err = recv(*sock, packet_size_str, 10, MSG_WAITALL);
        if (err==-1)
	{
                throw new string("send_channel_t::crecv error recv");
        }
        else if (err==0)
        {
                throw new string("send_channel_t::crecv: other node closed connection");
        }
        if (err<10)
        {
            throw new string ("send_channel_t::crecv less than 5 bytes");
        }
        if (err>10)
        {
            throw new string ("send_channel_t::crecv more than 5 bytes");
        }
        
        packet_size_str[8] = '\0';

        sscanf(packet_size_str, "%d", &packet_size);
        
         
        if (packet_size-2<0)
        {
            throw new string (string("send_channel_t::crecv anomal packet size ") );
        }
        if (packet_size-2==0)
            return string("");
        
       // cout << string("packet_size(") + packet_size_str + ")\n";
    
        
        char* packet = new char[packet_size];
        
        err = recv(*sock, packet, packet_size, MSG_WAITALL);
        if (err==-1)
	{
                throw new string("send_channel_t::crecv recv");
        }
        else if (err==0)
        {
                throw new string("send_channel_t::crecv: other node closed connection");
        }
        if (err<packet_size)
        {
            throw new string ("send_channel_t::crecv less than packet_size bytes");
        }
        if (err>packet_size)
        {
            throw new string ("send_channel_t::crecv more than packet_size bytes");
        }
       
        
        packet[packet_size-2] = '\0';
        string ret(packet);
        delete [] packet;

        
        //ret = ret.substr(0, ret.length()-1);
        //ret = ret.substr(0, ret.length()-1);
        //ret.erase(std::remove(ret.begin(), ret.end(), '\r'), ret.end());
        //ret.erase(std::remove(ret.begin(), ret.end(), '\n'), ret.end());

        
        /*while (ret.length()>packet_size-2)
        {
            ret = ret.substr(0, ret.length()-1);
        }*/

        if (ret == "send_channel_t::confirm_close")
        {
            //LOG("crecv:: otherside requested close protocol initiation");
            otherside_close_confirm_waiting = 1;
            if (!thisside_waiting_close)
                return crecv();
        }
        return ret;
}
/*
char* send_channel_t::crecv(char *bf)
{
        bf = new char[255];
	if (recv(*sock, bf, 255, 0)<=0)
	{
                //state = FAULT;
	}
        return bf;
}*/

send_channel_t::send_channel_t()
{
    otherside_close_confirm_waiting = 0;
    thisside_waiting_close = 0;
}

send_channel_t::~send_channel_t()
{

}

send_channel_t::send_channel_t(int *sock)
{
        thisside_waiting_close = 0;
        otherside_close_confirm_waiting = 0;
        this->sock = sock;
}

void send_channel_t::connect(string ip, int port)
{
        thisside_waiting_close = 0;
        otherside_close_confirm_waiting = 0;
        sock = hsock_t::client(ip.c_str(), port);
}

void send_channel_t::close()
{
    //shutdown(*sock, SHUT_RD);
    return;
    thisside_waiting_close = 1;
    if (otherside_close_confirm_waiting)
    {
        //LOG("confirming close to other side ");
        csend("send_channel_t::close_confirmed");
        shutdown(*sock, 2);
        //delete sock;
    }
    else
    {
        //LOG("send_channel_t::close initiating confirm close protocol ");
        csend("send_channel_t::confirm_close");
        //cout << "send_channel_t::close waiting for esponse\n";
        string ret = crecv();
        if (ret == "send_channel_t::close_confirmed")
        {
            //cout  << "close confirmed from otherside\n";
            //sleep(2);
            shutdown(*sock, 2);
            //delete sock;
        }
        else if (ret == "send_channel_t::confirm_close")
        {
            //cout << "confirming close to other side \n";
            csend("send_channel_t::close_confirmed");
            //sleep(2);
            shutdown(*sock, 2);
            //delete sock;
        }
        else
        {
            throw new string ("send_channel_t::close confirmation fail. possibly got user data. check your protocol");
        }
    }
}
