/* 
 * File:   hdivider_mongo_accessors.cpp
 * Author: phrk
 *
 * Created on June 22, 2013, 4:16 PM
 */

#include "hdivider_mongo_accessors.h"

void HdividerMongoInputIdIt::connect()
{
    int status = mongo_connect(conn, ip.c_str(), port);

    if( status != MONGO_OK )
    {
      switch ( conn->err )
      {
        case MONGO_CONN_SUCCESS:    {  break; }
        case MONGO_CONN_NO_SOCKET:  { throw new string ("HdividerMongoInputIdIt:: MONGO_CONN_NO_SOCKET" );  }
        case MONGO_CONN_FAIL:       { throw new string ("HdividerMongoInputIdIt:: MONGO_CONN_FAIL" );  }
        case MONGO_CONN_NOT_MASTER: { throw new string ("HdividerMongoInputIdIt:: MONGO_CONN_NOT_MASTER" );  }
      }
    }
    if ( mongo_cmd_authenticate(conn, db_name.c_str(), login.c_str(), pass.c_str()) == MONGO_ERROR )
    {
         throw new string ("HdividerMongoInputIdIt:: MONGO_ERROR error AUTH");
    }

    bson query[1];
    bson_init(query);
    bson_finish(query);
    this->coll_size = (int)mongo_count(conn, db_name.c_str(), coll_name.c_str(), query);
    bson_destroy(query);
}

HdividerMongoInputIdIt::HdividerMongoInputIdIt(string ip, int port, string db_name, string coll_name, string login, string pass)
{
    nskip = 0;
    this->coll_size = 0;
    this->db_name = db_name;
    isend = 1;
    this->coll_name = coll_name;
    conn = new mongo;
    this->ip = ip;
    this->login = login;
    this->pass = pass;
    this->port = port;
    connect();
    
    setFirst();
}

HdividerMongoInputIdIt::~HdividerMongoInputIdIt()
{
    mongo_cursor_destroy( obj_cursor );
    mongo_destroy( conn );
    delete conn;
}

int HdividerMongoInputIdIt::size()
{
    return this->coll_size;
}

void HdividerMongoInputIdIt::setFirst()
{
    mongo_cursor_init( obj_cursor, conn, (db_name+"."+coll_name).c_str() );
    getNext();
}

void HdividerMongoInputIdIt::getNext()
{        
    if ( mongo_cursor_next( obj_cursor ) == MONGO_OK )
    {
            bson_iterator iterator[1];
            if ( bson_find( iterator, mongo_cursor_bson( obj_cursor ), "id" ))
            {
                    cur_id = int (bson_iterator_int( iterator ));
                    isend = 0;
                    nskip++;
            }
    }
    else
    {
        cout << "input getNext error\n";
        bool ok = 0;
        for (int i = 0; i<3; i++)
        {
 
            cout << "reseting cursor\n";
           
            mongo_cursor_destroy(obj_cursor);
            mongo_cursor_init( obj_cursor, conn, (db_name+"."+coll_name).c_str() );
            mongo_cursor_set_skip (obj_cursor, nskip);
            bson_iterator iterator[1];
            
            if ( mongo_cursor_next( obj_cursor ) == MONGO_OK )
            {
                if ( mongo_cursor_next( obj_cursor ) == MONGO_OK )
                {
                    cout << "cursor reset ok. continuing\n";
                    if ( bson_find( iterator, mongo_cursor_bson( obj_cursor ), "id" ))
                    {
                        cout << "got next id \n";
                        cur_id = int (bson_iterator_int( iterator ));
                        isend = 0;
                        ok = 1;
                        nskip++;
                        break;
                    }
                }
            }
            
            // if not helped reconnect
            cout << "reconnecting\n";
            mongo_disconnect(conn);
            connect();
            
        }
        
        if (!ok)
        isend = 1;
    }
}

InputId HdividerMongoInputIdIt::value()
{
    return cur_id;
}

bool HdividerMongoInputIdIt::end()
{
    return isend;
}

////////////////////// HdividerMongoStateAccessor //////////////////////

HdividerMongoStateAccessor::HdividerMongoStateAccessor(string ip, int port, string db_name, string job_name, string login, string pass)
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
            case MONGO_CONN_NO_SOCKET:  { throw new string ("HdividerMongoStateAccessor:: MONGO_CONN_NO_SOCKET" ); return; }
            case MONGO_CONN_FAIL:       { throw new string ("HdividerMongoStateAccessor:: MONGO_CONN_FAIL" ); return; }
            case MONGO_CONN_NOT_MASTER: { throw new string ("HdividerMongoStateAccessor:: MONGO_CONN_NOT_MASTER" ); return; }
          }
      }
      if ( mongo_cmd_authenticate(conn, db_name.c_str(), login.c_str(), pass.c_str()) == MONGO_ERROR )
      {
             throw new string ("HdividerMongoStateAccessor:: MONGO_ERROR error AUTH");
             return;
      }
}

InputState* HdividerMongoStateAccessor::getState(int64_t input_id)
{     
    bson query[1];
    bson_init(query);
    bson_append_int(query, "id", input_id);
    bson_finish(query);

    InputState *state = new InputState();

    mongo_cursor cursor[1];

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

void HdividerMongoStateAccessor::saveState(const InputState *state)
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

void HdividerMongoStateAccessor::resetState()
{
    bson b[1];
    bson_init(b);
    bson_finish(b);
    mongo_remove(conn, (db_name+"."+coll_name).c_str(), b, NULL);
    bson_destroy(b);
}

tr1::unordered_map<int64_t, InputState*>* HdividerMongoStateAccessor::getAllStates()
{
    tr1::unordered_map<int64_t, InputState*>* states = new tr1::unordered_map<int64_t, InputState*>;
    mongo_cursor cursor[1];
    mongo_cursor_init( cursor, conn, (db_name+"."+coll_name).c_str() );
    
    
    while ( mongo_cursor_next( cursor ) == MONGO_OK )
    {
            InputState *state = new InputState;
            bson_iterator iterator[1];
            if ( bson_find( iterator, mongo_cursor_bson( cursor ), "id" ))
            {
                    state->id =  (bson_iterator_long( iterator ));            
            }
            if ( bson_find( iterator, mongo_cursor_bson( cursor ), "locked_by" ))
            {
                    state->locked_by = string (bson_iterator_string( iterator ));            
            }
            if ( bson_find( iterator, mongo_cursor_bson( cursor ), "handled" ))
            {
                    state->handled = bool (bson_iterator_int( iterator ));            
            }
            states->insert( pair<int64_t, InputState*>(state->id, state) );
    }
    
    mongo_cursor_destroy(cursor);
    return states;
}