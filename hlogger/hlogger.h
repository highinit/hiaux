/* 
 * File:   hlogger.h
 * Author: phrk
 *
 * Created on May 29, 2013, 12:01 AM
 */

#ifndef HLOGGER_H
#define	HLOGGER_H

#include <string>

#define MONGO_HAVE_STDINT
#include "mongo.h"

using namespace std;

#define EVENT0 0
#define EVENT1 1
#define WARNING 2
#define ERROR 3

class Hlogger
{
    mongo* conn;
    string db_name;
    string coll_name;
    string worker_id;
    
public:
    
    Hlogger(string ip, int port, string db_name, string coll_name, string worker_id, string login, string pass)
    {
        this->db_name = db_name;
        this->coll_name = coll_name;
        this->worker_id = worker_id;
        conn = new mongo;

        int status = mongo_client(conn, ip.c_str(), port);

          if( status != MONGO_OK )
          {
              switch ( conn->err )
              {
                case MONGO_CONN_SUCCESS:    {  break; }
                case MONGO_CONN_NO_SOCKET:  { throw new string ("Hlogger:: MONGO_CONN_NO_SOCKET" ); return; }
                case MONGO_CONN_FAIL:       { throw new string ("Hlogger:: MONGO_CONN_FAIL" ); return; }
                case MONGO_CONN_NOT_MASTER: { throw new string ("Hlogger:: MONGO_CONN_NOT_MASTER" ); return; }
              }
          }
          if ( mongo_cmd_authenticate(conn, db_name.c_str(), login.c_str(), pass.c_str()) == MONGO_ERROR )
          {
                 throw new string ("Hlogger:: MONGO_ERROR error AUTH");
                 return;
          }
    }
    
    void log(const int type, const string event)
    {
        bson bevent[1];
        bson_init(bevent);
        bson_append_string(bevent, "node", worker_id.c_str());
        bson_append_int(bevent, "type", type);
        bson_append_string(bevent, "event", event.c_str());     
        bson_finish(bevent);
        
        mongo_insert(conn, (db_name+"."+coll_name).c_str(), bevent, NULL);
        
        bson_destroy(bevent);
    }
};


#endif	/* HLOGGER_H */

