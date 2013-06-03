/* 
 * File:   main.cpp
 * Author: phrk
 *
 * Created on May 16, 2013, 10:50 PM
 */

#include <cstdlib>

using namespace std;

#include "../meroving/hcomm/include/common.h"
#include "../meroving/hcomm/include/hcomm.h"
#include "../meroving/hcomm/include/hcomms.h"
#include "../hconfig_parser/hconfig_parser.h"
#include "../hlogger/hlogger.h"

class NsLauncher
{
    hcomm_srv_t *ns;
public:
    
    void launch(map<string, string>* vars) 
    {
        vector<string> keys;
        keys.push_back("ns_port");
        keys.push_back("nss_port");
        keys.push_back("nse_port");
        
        if (!HconfigParser::existVars(vars, keys))
        {
            throw new string("NsLauncher::launch ERROR not all params set");
        }
        
        int ns_port, nss_port, nse_port, db_port;
        sscanf((*vars)["ns_port"].c_str(), "%d", &ns_port);
        sscanf((*vars)["nss_port"].c_str(), "%d", &nss_port);
        sscanf((*vars)["nse_port"].c_str(), "%d", &nse_port);
        sscanf((*vars)["db_port"].c_str(), "%d", &db_port);
        
        //cout << ns_port << " " << nss_port << " " << nse_port << endl;
        
        Hlogger *logger = new Hlogger((*vars)["db_ip"], db_port, (*vars)["db_name"], "logs", (*vars)["worker_id"], (*vars)["db_user"], (*vars)["db_pass"]);
        ns = new hcomm_srv_t(logger);
        ns->start_server(ns_port, nss_port, nse_port);
        
    }
    
    void kill()
    {
        ns->kill_server();
    }
    
    void join()
    {
        ns->join();
    }
};

/*
 * 
 */
int main(int argc, char** argv) 
{
    
    NsLauncher ns;
    ns.launch(HconfigParser::load("ns.conf"));
    ns.join();
    

    return 0;
}

