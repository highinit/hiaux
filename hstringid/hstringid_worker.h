/* 
 * File:   hstringid_worker.h
 * Author: phrk
 *
 * Created on June 16, 2013, 1:13 PM
 */

#ifndef HSTRINGID_WORKER_H
#define	HSTRINGID_WORKER_H

#include "master/hword_master.h"
#include "node/hword_node.h"

#include "../hrpc/hcomm/include/hcomm.h"
#include "hword_master_skel.h"
#include "hword_db_accessor.h"

#include "../hconfig_parser/hconfig_parser.h"

class HstringIdWorker
{
public:
    static void start(map<string, string>* vars)
    {
        vector<string> keys; 
        keys.push_back("db_ip");
        keys.push_back("db_port");
        keys.push_back("db_name");	
        keys.push_back("db_user");	
        keys.push_back("db_pass");	
        
        keys.push_back("out_ids_coll");	
        
        keys.push_back("worker_id");	
        keys.push_back("ns_ip");
        keys.push_back("ns_port");
        keys.push_back("cache_enabled");

        if (!HconfigParser::existVars(vars, keys))
        {
            throw new string("HstringIdWorker::start ERROR not all params set\n");
        }
        ////////
        
        cout << "HstringID master " + (*vars)["out_ids_coll"] << endl;
        
        int db_port;
        sscanf((*vars)["db_port"].c_str(), "%d", &db_port);
        
        int ns_port;
        sscanf((*vars)["ns_port"].c_str(), "%d", &ns_port);
        
        int cache_enabled;
        sscanf((*vars)["cache_enabled"].c_str(), "%d", &cache_enabled);
        
        if (cache_enabled==CACHE_ENABLED)
        {
            cout << "cache enabled" << endl;
        }
        else
        {
            cout << "cache disabled" << endl;
        }
        
        Hlogger *logger = new Hlogger((*vars)["db_ip"], \
                                        db_port, \
                                        (*vars)["db_name"], \
                                        "logs", \
                                        (*vars)["worker_id"]+"_hstringid_master", \
                                        (*vars)["db_user"], \
                                        (*vars)["db_pass"]);
        
        hcomm_t share_comm((*vars)["db_ip"], ns_port, "hstringid_master_"+(*vars)["out_ids_coll"], logger);
        HwordMongoDbAccessor *db_read = new HwordMongoDbAccessor((*vars)["db_ip"], \
                                                                db_port, \
                                                                (*vars)["db_name"], \
                                                                (*vars)["out_ids_coll"], \
                                                                (*vars)["db_user"], \
                                                                (*vars)["db_pass"]); 
        
        HwordMongoDbAccessor *db_write = new HwordMongoDbAccessor((*vars)["db_ip"], \
                                                                db_port, \
                                                                (*vars)["db_name"], \
                                                                (*vars)["out_ids_coll"], \
                                                                (*vars)["db_user"], \
                                                                (*vars)["db_pass"]); 
        
        HwordMaster *master = new HwordMaster(db_read, db_write, cache_enabled);
        share_comm.connect();
        
        share_comm.share_obj<HwordMaster, HwordMasterSkel>(master, "hstringid_"+(*vars)["out_ids_coll"]);
        share_comm.start_server();
        share_comm.join();
    }
};

#endif	/* HSTRINGID_WORKER_H */

