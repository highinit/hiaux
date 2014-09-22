#ifndef _HTTP_SIMPLE_REQUESTER_POST_H_
#define _HTTP_SIMPLE_REQUESTER_POST_H_

#include "hiconfig.h"
#include "hiaux/network/HttpOutReqDisp.h"
//#include "hiaux/network/HttpSrv.h"
#include "hiaux/network/CallContext.h"

/*
class CallContextPost {

public:
	virtual ~CallContextPost() { }
};

typedef boost::shared_ptr<CallContextPost> CallContextPPtr;
*/
class HttpSimpleRequesterPost : public HttpOutRequestDisp::Requester {
public:	

private:
	std::string m_call_url;
	std::string m_postdata;
	boost::function<void(CallContextPtr, const std::string&)> m_onCalledOk;
	boost::function<void(CallContextPtr)> m_onCalledFail;
	CallContextPtr m_context;

public:
	
	HttpSimpleRequesterPost(boost::function<void(int, int, const std::string&)> _onCall,
							boost::function<void(int, int, const std::string&, const std::string&)> _onCallPost,
							boost::function<void(int)> _onFinished,
							CallContextPtr _context,
							const std::string &_call_url,
							const std::string &_postdata,
							boost::function<void(CallContextPtr, const std::string&)> _onCalledOk,
							boost::function<void(CallContextPtr)> _onCalledFail);
	
	virtual void onCallDone(int _callid, bool _success, const std::string &_resp);
	virtual void start();
};

#endif
