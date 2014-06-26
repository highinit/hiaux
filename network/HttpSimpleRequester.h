#ifndef _HTTP_SIMPLE_REQUESTER_
#define _HTTP_SIMPLE_REQUESTER_

#include "hiaux/network/HttpOutReqDisp.h"
#include "hiaux/network/HttpSrv.h"

class CallContext {

public:
	virtual ~CallContext() { }
};

typedef boost::shared_ptr<CallContext> CallContextPtr;

class HttpSimpleRequester : public HttpOutRequestDisp::Requester {
public:	

private:
	std::string m_call_url;
	boost::function<void(CallContextPtr, const std::string&)> m_onCalledOk;
	boost::function<void(CallContextPtr)> m_onCalledFail;
	CallContextPtr m_context;

public:
	
	HttpSimpleRequester(boost::function<void(int, int, const std::string&)> _onCall,
							boost::function<void(int)> _onFinished,
							CallContextPtr _context,
							const std::string &_call_url,
							boost::function<void(CallContextPtr, const std::string&)> _onCalledOk,
							boost::function<void(CallContextPtr)> _onCalledFail);
	
	virtual void onCallDone(int _callid, bool _success, const std::string &_resp);
	virtual void start();
};

#endif
