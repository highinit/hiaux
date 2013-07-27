/* 
 * File:   hdivider_mongo_accessors.h
 * Author: phrk
 *
 * Created on April 28, 2013, 11:04 PM
 */

#ifndef hdivider_mongo_accessors
#define	hdivider_mongo_accessors

#define MONGO_HAVE_STDINT
#include "mongo.h"
#include <string>
#include <tr1/unordered_map>
#include "../hdivider_core/include/hdivider_watcher.h"

using namespace std;

class HdividerMongoInputIdIt : public HdividerInputIdIt 
{
    int nskip;
    
    int cur_id;
    string db_name;
    string coll_name;
    
    mongo *conn;
    mongo_cursor obj_cursor[1];
    
    bool isend;
    int coll_size;
    string ip;
    string login;
    string pass;
    int port;
    void connect();
    
public:
    
    HdividerMongoInputIdIt(string ip, int port, string db_name, string coll_name, string login, string pass);  
    ~HdividerMongoInputIdIt();
    
    virtual int         size();
    virtual void        setFirst();
    virtual void        getNext();
    virtual int64_t     value();
    virtual bool        end();
};

class HdividerMongoStateAccessor : public HdividerStateAccessor
{
    string db_name;
    string coll_name;
    
    mongo *conn;
       
public:
   
    HdividerMongoStateAccessor(string ip, int port, string db_name, string job_name, string login, string pass);
      
    virtual InputState*                                 getState(int64_t input_id);
    tr1::unordered_map<int64_t, InputState*>*           getAllStates();
    virtual void                                        saveState(const InputState *state);
    virtual void                                        resetState();
};

#endif	/* hdivider_mongo_accessors */

