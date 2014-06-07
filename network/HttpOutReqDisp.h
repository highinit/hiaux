#ifndef _HTTP_OUT_REQ_DISP_H_
#define _HTTP_OUT_REQ_DISP_H_

#include <ecl/ecl.h>
#include <boost/function.hpp>
#include "hiaux/network/HttpSrv.h"
#include "hiaux/network/HttpClientAsync.h"

class HttpOutRequestDisp {
public:
	
	class Requester {
		
		boost::function<void(int, int, const std::string&)> m_onCall;
		boost::function<void(int)> m_onFinished;
	protected:
		int m_id;
	public:
		
		Requester(boost::function<void(int, int, const std::string&)> _onCall,
					boost::function<void(int)> _onFinished,
					int id);
		
		void call (int _callid, const std::string &_url);
		void finished();
		virtual void onCallDone (int _callid, bool _success, const std::string &_resp) = 0;
		virtual void start() = 0;
		int getId();
	};

	typedef boost::shared_ptr<Requester> RequesterPtr;

	class OutRequestInfo {
	public:
		OutRequestInfo(int _reqid, int _requester_callid);
		int reqid;
		int requester_callid;
	};
	
	// requester_id / formatid
	hiaux::hashtable<int, RequesterPtr> m_requesters;
	HttpClientAsyncPtr m_http_client;
	TaskLauncherPtr m_launcher;
	hAutoLock lock;
	
public:
	
	TaskLauncher::TaskRet callDoneNotify(HttpOutRequestDisp::RequesterPtr _req, int callid, bool success, std::string resp);
	TaskLauncher::TaskRet callStart(HttpOutRequestDisp::RequesterPtr _req);
	
	HttpOutRequestDisp(TaskLauncherPtr _launcher);
	void onCall(int _reqid, int _reqcallid, const std::string &_url);
	void onCallDone(HttpClientAsync::JobInfo _ji);
	void onRequesterFinished(int _reqid);
	
	void addRequester(RequesterPtr _requester);
	
	void kick();
};

typedef boost::shared_ptr<HttpOutRequestDisp> HttpOutRequestDispPtr;

#endif
