/* 
 * File:   hdivider_mongo_accessors.h
 * Author: phrk
 *
 * Created on April 28, 2013, 11:04 PM
 */

#ifndef hdivider_mongo_accessors
#define	hdivider_mongo_accessors

#define MONGO_USE__INT64
#include "mongo.h"


class HdividerMongoInputIdIt : public HdividerInputIdIt 
{
    
    int cur_id;
    string db_name;
    string coll_name;
    
    mongo *conn;
    mongo_cursor obj_cursor[1];
    
    bool isend;
    
    int coll_size;
    
public:
    
    HdividerMongoInputIdIt(string ip, int port, string db_name, string coll_name, string login, string pass)
    {
        this->coll_size = 0;
        this->db_name = db_name;
        isend = 1;
        this->coll_name = coll_name;
        conn = new mongo;
        
        int status = mongo_connect(conn, ip.c_str(), port);

          if( status != MONGO_OK )
          {
              switch ( conn->err )
              {
                case MONGO_CONN_SUCCESS:    {  break; }
                case MONGO_CONN_NO_SOCKET:  { throw new string ("HdividerMongoInputIdIt:: MONGO_CONN_NO_SOCKET" ); return; }
                case MONGO_CONN_FAIL:       { throw new string ("HdividerMongoInputIdIt:: MONGO_CONN_FAIL" ); return; }
                case MONGO_CONN_NOT_MASTER: { throw new string ("HdividerMongoInputIdIt:: MONGO_CONN_NOT_MASTER" ); return; }
              }
          }
          if ( mongo_cmd_authenticate(conn, db_name.c_str(), login.c_str(), pass.c_str()) == MONGO_ERROR )
          {
                 throw new string ("HdividerMongoInputIdIt:: MONGO_ERROR error AUTH");
                 return;
          }
        
        bson query[1];
        bson_init(query);
        bson_finish(query);
        this->coll_size = (int)mongo_count(conn, db_name.c_str(), coll_name.c_str(), query);
        bson_destroy(query);
        
        setFirst();
    }
    
    ~HdividerMongoInputIdIt()
    {
        mongo_cursor_destroy( obj_cursor );
        mongo_destroy( conn );
        delete conn;
    }
    
    virtual int size()
    {
        return this->coll_size;
    }
    
    virtual void setFirst()
    {
        mongo_cursor_init( obj_cursor, conn, (db_name+"."+coll_name).c_str() );
        getNext();
    }
    
    virtual void getNext()
    {        
        if ( mongo_cursor_next( obj_cursor ) == MONGO_OK )
        {
                bson_iterator iterator[1];
                if ( bson_find( iterator, mongo_cursor_bson( obj_cursor ), "id" ))
                {
                        cur_id = int (bson_iterator_int( iterator ));
                        isend = 0;
                }
        }
        else
        {
            isend = 1;
        }
    }
    
    virtual InputId value()
    {
        return cur_id;
    }
    
    virtual bool end()
    {
        return isend;
    }
};

class HdividerMongoStateAccessor : public HdividerStateAccessor
{
   // map<InputId, InputState*> states;
   // pthread_mutex_t state_mutex;
    
    string db_name;
    string coll_name;
    
    mongo *conn;
    
    
public:
   
    HdividerMongoStateAccessor(string ip, int port, string db_name, string job_name, string login, string pass)
    {
        this->db_name = db_name;
        this->coll_name = "job_state_"+job_name;
        conn = new mongo;
        
        int status = mongo_connect(conn, ip.c_str(), port);

          if( status != MONGO_OK )
          {
              switch ( conn->err )
              {
                case MONGO_CONN_SUCCESS:    {  break; }
                case MONGO_CONN_NO_SOCKET:  { throw new string ("HdividerMongoInputIdIt:: MONGO_CONN_NO_SOCKET" ); return; }
                case MONGO_CONN_FAIL:       { throw new string ("HdividerMongoInputIdIt:: MONGO_CONN_FAIL" ); return; }
                case MONGO_CONN_NOT_MASTER: { throw new string ("HdividerMongoInputIdIt:: MONGO_CONN_NOT_MASTER" ); return; }
              }
          }
          if ( mongo_cmd_authenticate(conn, db_name.c_str(), login.c_str(), pass.c_str()) == MONGO_ERROR )
          {
                 throw new string ("HdividerMongoInputIdIt:: MONGO_ERROR error AUTH");
                 return;
          }
        
        //pthread_mutex_init(&state_mutex, 0);
    }
    
    virtual InputState *getState(InputId input_id)
    {     
        bson query[1];
        bson_init(query);
        bson_append_int(query, "id", input_id);
        bson_finish(query);
        
        InputState *state = new InputState();
        
        mongo_cursor cursor[1];// = mongo_find(conn, (db_name+"."+coll_name).c_str(), query, NULL, 1, 0, 0 );
        
        mongo_cursor_init( cursor, conn, (db_name+"."+coll_name).c_str() );
        mongo_cursor_set_query( cursor, query );

        if (mongo_cursor_next( cursor ) == MONGO_OK )
        {
                state->id = input_id;
                bson_iterator iterator[1];
                if ( bson_find( iterator, mongo_cursor_bson( cursor ), "locked_by" ))
                {
                        state->locked_by = string (bson_iterator_string( iterator ));            
                }
                if ( bson_find( iterator, mongo_cursor_bson( cursor ), "handled" ))
                {
                        state->handled = bool (bson_iterator_int( iterator ));            
                }
                mongo_cursor_destroy(cursor);
                bson_destroy( query );
                return state; 
        }
        else
        {
            state->id = input_id;
            state->locked_by = "";
            state->handled = 0;
            
            saveState(state);
            
            return state;
        }

    }
    
    virtual void saveState(InputState *state)
    {
        bson query[1];
        bson_init(query);
        bson_append_int(query, "id", state->id);
        bson_finish(query);
        
        bson b[1];
        bson_init(b);
        bson_append_int(b, "id", state->id);
        bson_append_string(b, "locked_by", state->locked_by.c_str());
        bson_append_int(b, "handled", (int)state->handled);
        bson_finish(b);
  
        mongo_cursor cursor[1];
        
        mongo_cursor_init( cursor, conn, (db_name+"."+coll_name).c_str() );
        mongo_cursor_set_query( cursor, query );

        if (mongo_cursor_next( cursor ) == MONGO_OK )
        {
                mongo_update( conn, (db_name+"."+coll_name).c_str(), query, b, 0, NULL );
        }
        else
        {
                mongo_insert( conn, (db_name+"."+coll_name).c_str(), b, NULL );
        }
        
        mongo_cursor_destroy(cursor);
        bson_destroy(query);
        bson_destroy(b);       
    }
    
    virtual void resetState()
    {
        //pthread_mutex_lock(&mutex);
        //states.clear();
       // pthread_mutex_unlock(&mutex);
    }
};

#endif	/* hdivider_mongo_accessors */

