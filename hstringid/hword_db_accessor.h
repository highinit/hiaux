#include "master/hword_master.h"
#define MONGO_HAVE_STDINT
#include "mongo.h"

class HwordMongoDbAccessor : public HwordDbAccessor
{
    mongo *conn;
    string db_name;
    string coll_name;
public:
    
    HwordMongoDbAccessor(string ip, int port, string db_name, string coll_name, string login, string pass)
    {

        this->db_name = db_name;

        this->coll_name = coll_name;
        conn = new mongo;
        
        for (int i = 0; i<6; i++)
        {
            try
            {
                int status = mongo_client(conn, ip.c_str(), port);

                if( status != MONGO_OK )
                {
                  switch ( conn->err )
                  {
                    case MONGO_CONN_SUCCESS:    {  break; }
                    case MONGO_CONN_NO_SOCKET:  { throw new string ("HwordMongoDbAccessor:: MONGO_CONN_NO_SOCKET" ); return; }
                    case MONGO_CONN_FAIL:       { throw new string ("HwordMongoDbAccessor:: MONGO_CONN_FAIL" ); return; }
                    case MONGO_CONN_NOT_MASTER: { throw new string ("HwordMongoDbAccessor:: MONGO_CONN_NOT_MASTER" ); return; }
                  }
                }
                if ( mongo_cmd_authenticate(conn, db_name.c_str(), login.c_str(), pass.c_str()) == MONGO_ERROR )
                {
                     throw new string ("HwordMongoDbAccessor:: MONGO_ERROR error AUTH");
                     return;
                }
                break;
            }
            catch(string *s)
            {
                cout << s->c_str();
                cout << ".. reconnecting\n";
                
                if (i==5)
                    throw s;
                sleep(2);
                continue;
            }
                
        }
    }
    
    virtual tr1::unordered_map<string, int64_t> *getIds()
    {
        tr1::unordered_map<string, int64_t> *ids = new tr1::unordered_map<string, int64_t>;
        mongo_cursor cursor[1];
        mongo_cursor_init( cursor, conn, (db_name+"."+coll_name).c_str() );
        while ( mongo_cursor_next( cursor ) == MONGO_OK )
        {
                bson_iterator iterator[1];
                int64_t cur_id;
                string cur_word;
                if ( bson_find( iterator, mongo_cursor_bson( cursor ), "word" ))
                {
                    cur_word = string (bson_iterator_string( iterator ));
                }
                if ( bson_find( iterator, mongo_cursor_bson( cursor ), "id" ))
                {
                        cur_id = int64_t (bson_iterator_long( iterator ));
                }
                ids->insert(pair<string, int64_t>(cur_word, cur_id));
        }
        return ids;
    }
    
    virtual void savePair(string word, int64_t id)
    {
        bson b[1];
        bson_init(b);
        bson_append_string(b, "word", word.c_str());
        bson_append_long(b, "id", id);
        bson_finish(b);
        mongo_insert( conn, (db_name+"."+coll_name).c_str(), b, NULL );
        bson_destroy(b);
    }
    
    string getWord(int64_t id)
    {
        string word = "";
        bson query[1];
        bson_init(query);
        bson_append_long(query, "id", id);
        bson_finish(query);
        
        mongo_cursor cursor[1];
        
        mongo_cursor_init( cursor, conn, (db_name+"."+coll_name).c_str() );
        mongo_cursor_set_query( cursor, query );

        if (mongo_cursor_next( cursor ) == MONGO_OK )
        {
                bson_iterator iterator[1];
                if ( bson_find( iterator, mongo_cursor_bson( cursor ), "word" ))
                {
                        word = string (bson_iterator_string( iterator ));            
                }
        }
        bson_destroy(query);
        mongo_cursor_destroy(cursor);
        return word;
    }
    
    virtual int64_t getId(string word)
    {
        int64_t id;
        bson query[1];
        bson_init(query);
        bson_append_string(query, "word", word.c_str());
        bson_finish(query);
        
        mongo_cursor cursor[1];
        
        mongo_cursor_init( cursor, conn, (db_name+"."+coll_name).c_str() );
        mongo_cursor_set_query( cursor, query );
        
        if (mongo_cursor_next( cursor ) == MONGO_OK )
        {
                bson_iterator iterator[1];
                if ( bson_find( iterator, mongo_cursor_bson( cursor ), "id" ))
                {
                        id = int64_t (bson_iterator_long( iterator ));            
                }
                else
                {
                    throw "id not found";
                }
        }
        else
        {
            bson_destroy(query);
            mongo_cursor_destroy(cursor);
            throw new string ("HwordMongoDbAccessor::getId word " + word + " not found in db");
        }
        bson_destroy(query);
        mongo_cursor_destroy(cursor);
        return id;
    }
    
    void clearAll()
    {
        bson b[1];
        bson_init(b);
        bson_finish(b);
        mongo_remove(conn, (db_name+"."+coll_name).c_str(), b, NULL);
        bson_destroy(b);
    }
    
};