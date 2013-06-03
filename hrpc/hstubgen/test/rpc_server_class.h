
#ifndef RPCSERVER
#define RPCSERVER

#include <vector>

using namespace std;

#define LOG(str) hlog_log(str, "hcomm.log")

class RpcServer
{
public:
    //#remote
    void handleFault(string worker_id)
    {
    
    }
    //#remote
    int isFinished()
    {
        LOG( string("____call isFinished "));
        return 1;
    }
    //#remote
    vector<int> getInput(int count, string worker_id)
    {
        vector<int> ret;
        ret.push_back(count);
        ret.push_back(count+1);
        ret.push_back(count+2);
        char bf[255];
        sprintf(bf, "%d", count);
        LOG( string("____call getInput ") + bf + " " + worker_id);
        return ret;
    }
    //#remote
    void setHandled(int input_id)
    {
        char bf[255];
        sprintf(bf, "%d", input_id);
        LOG( string("____call setHandled ") + bf);
    }
    //#remote
    void lockResult(int result_id, string worker_id)
    {
    
    }
    //#remote
    void unlockResult(int result_id, string worker_id)
    {
    
    }
};

#endif