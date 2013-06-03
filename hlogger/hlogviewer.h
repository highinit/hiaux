/* 
 * File:   hlogviewer.h
 * Author: phrk
 *
 * Created on May 30, 2013, 2:46 AM
 */

#ifndef HLOGVIEWER_H
#define	HLOGVIEWER_H

#include <string>
#include <vector>
using namespace std;

#include "mongo.h"

class LogEvent
{
public:
    string node;
    int type;
    string event;
    
    LogEvent(const string node, const int type, const string event)
    {
        this->node = node;
        this->type = type;
        this->event = event;
    }
};

class HlogViewer
{
    string db_name;
    string coll_name;
    mongo *conn;
public:
    
    HlogViewer(string ip, int port, string db_name, string coll_name, \
            string login, string pass)
    {
        this->db_name = db_name;
        this->coll_name = coll_name;
        conn = new mongo;

        int status = mongo_client(conn, ip.c_str(), port);

          if( status != MONGO_OK ) {
              switch ( conn->err ) {
                case MONGO_CONN_SUCCESS:    {  break; }
                case MONGO_CONN_NO_SOCKET:  { throw new string ("Hlogger:: MONGO_CONN_NO_SOCKET" ); return; }
                case MONGO_CONN_FAIL:       { throw new string ("Hlogger:: MONGO_CONN_FAIL" ); return; }
                case MONGO_CONN_NOT_MASTER: { throw new string ("Hlogger:: MONGO_CONN_NOT_MASTER" ); return; }
              }
          }
          if ( mongo_cmd_authenticate(conn, db_name.c_str(), login.c_str(), pass.c_str()) == MONGO_ERROR ) {
                 throw new string ("Hlogger:: MONGO_ERROR error AUTH");
                 return;
          }
    }
    
    vector<LogEvent> doQuery(const string node, const int type, \
        const string pattern, const int nlines) const
    {
        vector<LogEvent> lines;
        bson query[1];
        bson_init(query);
        bson_append_string(query, "node", node.c_str());
        bson_append_int(query, "type", type);
        bson_finish(query);
        
        mongo_cursor cursor[1];

        mongo_cursor_init( cursor, conn, (db_name+"."+coll_name).c_str() );
        mongo_cursor_set_query( cursor, query );

        bson_destroy(query);
        
        while (mongo_cursor_next( cursor ) == MONGO_OK ) {
            string node;
            int type;
            string line;
            
            bson_iterator iterator[1];
            if ( bson_find( iterator, mongo_cursor_bson( cursor ), "node" )) {
                node = string(bson_iterator_string(iterator));
            } else {
                throw new string ("ERROR reading bson object");
            }
            if ( bson_find( iterator, mongo_cursor_bson( cursor ), "type" )) {
                type = int(bson_iterator_int(iterator));
            } else {
                throw new string ("ERROR reading bson object");
            }
            if ( bson_find( iterator, mongo_cursor_bson( cursor ), "event" )) {
                line = string(bson_iterator_string(iterator));
            } else {
                throw new string ("ERROR reading bson object");
            }
            
            lines.push_back(LogEvent(node, type, line));
        }
        
        mongo_cursor_destroy(cursor);
        return lines;
    }
};

#endif	/* HLOGVIEWER_H */

