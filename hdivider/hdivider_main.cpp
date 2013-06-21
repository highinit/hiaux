/* 
 * File:   hdivider_main.cpp
 * Author: phrk
 *
 * Created on May 18, 2013, 2:23 PM
 */

#define MONGO_HAVE_UNISTD
#include <cstdlib>

#include "hdivider.h"
#include "../hconfig_parser/hconfig_parser.h"

using namespace std;

class HdividerLauncher
{
    hcomm_t *comm_sharing;
public:
    void launch(map<string, string>* vars)
    {
        vector<string> keys;
        keys.push_back("db_ip");
        keys.push_back("db_port");
        keys.push_back("db_name");	
        keys.push_back("db_user");	
        keys.push_back("db_pass");	
        keys.push_back("in_coll");	
        keys.push_back("hdivider_job_id");
        keys.push_back("ns_ip");
        keys.push_back("ns_port");
        
        if (!HconfigParser::existVars(vars, keys))
        {
            throw new string("HdividerLauncher::launch ERROR not all params set");
        }
        
        int db_port;
        sscanf((*vars)["db_port"].c_str(), "%d", &db_port);
        
        int ns_port;
        sscanf((*vars)["ns_port"].c_str(), "%d", &ns_port);
        
        HdividerMongoStateAccessor *state_accessor = \
                new HdividerMongoStateAccessor((*vars)["db_ip"], db_port, (*vars)["db_name"],\
            (*vars)["hdivider_job_id"], (*vars)["db_user"], (*vars)["db_pass"]); 

        HdividerMongoInputIdIt *input_it = new HdividerMongoInputIdIt((*vars)["db_ip"], db_port, (*vars)["db_name"],\
            (*vars)["in_coll"], (*vars)["db_user"], (*vars)["db_pass"]);

        HdividerWatcher* watcher = new HdividerWatcher(input_it, state_accessor);
        
        Hlogger *logger = new Hlogger((*vars)["db_ip"], db_port, (*vars)["db_name"], "logs", (*vars)["hdivider_job_id"], (*vars)["db_user"], (*vars)["db_pass"]);
        
        try
        {
                comm_sharing = new hcomm_t((*vars)["ns_ip"], ns_port, (*vars)["hdivider_job_id"], logger);
                comm_sharing->connect();
        }
        catch (string *s)
        {
            throw new string("comm_sharing->connect " + *s);
        }
        
        try
        {
                comm_sharing->share_obj<HdividerWatcher, HdividerWatcherSkel>(watcher, (*vars)["hdivider_job_id"]);
                comm_sharing->start_server();
        }
        catch (string *s)
        {
                throw new string("comm_sharing->share_obj " + *s);
        }
        
    }
    
    void kill()
    {
        comm_sharing->kill_server();
    }
    
    void join()
    {
        comm_sharing->join();  
    }
};

int main(int argc, char** argv) 
{
    try
    {
        HdividerLauncher hdivider_launcher;
        ///divider_launcher.launch(HconfigParser::load("htext_ider_hdivider.conf"));
        //hdivider_launcher.launch(HconfigParser::load("hcrawler_hdivider.conf"));
        //hdivider_launcher.launch(HconfigParser::load("hindexer_hdivider.conf"));
        //hdivider_launcher.launch(HconfigParser::load("hindex_reducer_hdivider.conf"));
        hdivider_launcher.launch(HconfigParser::load(argv[1]));
        hdivider_launcher.join();
    }
    catch (string *s)
    {
        cout << *s;
    }
    
    return 0;
}

