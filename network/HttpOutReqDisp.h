#ifndef _HTTP_OUT_REQ_DISP_H_
#define _HTTP_OUT_REQ_DISP_H_

#include <boost/function.hpp>
#include "hiaux/threads/tasklauncher.h"
#include "hiaux/network/HttpClientAsync.h"

class HttpOutRequestDisp {
public:
	
	class Requester {
		
		boost::function<void(int, int, const std::string&)> m_onCall;
		boost::function<void(int, int, const std::string&, const std::string&)> m_onCallPost;
		boost::function<void(int)> m_onFinished;
	protected:
		int m_id;
		bool m_isfinished;
	public:
		
		Requester(boost::function<void(int, int, const std::string&)> _onCall,
					boost::function<void(int, int, const std::string&, const std::string&)> _onCallPost,
					boost::function<void(int)> _onFinished);
		
		void call (int _callid, const std::string &_url);
		void callPost (int _callid, const std::string &_url, const std::string &_postdata);
		bool finished();
		virtual void onCallDone (int _callid, bool _success, const std::string &_resp) = 0;
		virtual void start() = 0;
		void setId(int _id);
		int getId();
	};

	typedef boost::shared_ptr<Requester> RequesterPtr;

	class OutRequestInfo {
	public:
		OutRequestInfo(int _reqid, int _requester_callid);
		int reqid;
		int requester_callid;
	};
	
	int m_req_id;
	
	// requester_id / formatid
	hiaux::hashtable<int, RequesterPtr> m_requesters;
	HttpClientAsyncPtr m_http_client;
	TaskLauncherPtr m_launcher;
	hAutoLock lock;
	bool kick_running;
	bool kick_stopped;
	
	hCondWaiter cond_kicking;
	//hAutoLock kicklock;
public:
	
	bool isKickStopped();
	
	TaskLauncher::TaskRet onCallTask(int _reqid, int _reqcallid, const std::string &_url);
	TaskLauncher::TaskRet onCallPostTask(int _reqid, int _reqcallid, const std::string &_url, const std::string &_postdata);
	TaskLauncher::TaskRet onCallDoneTask(HttpClientAsync::JobInfo _ji);
	TaskLauncher::TaskRet onRequesterFinishedTask(int _reqid);
	TaskLauncher::TaskRet addRequesterTask(HttpOutRequestDisp::RequesterPtr _req);
	
	TaskLauncher::TaskRet kickTask();
	
	HttpOutRequestDisp(TaskLauncherPtr _launcher);
	~HttpOutRequestDisp();
	
	void onCall(int _reqid, int _reqcallid, const std::string &_url);
	void onCallPost(int _reqid, int _reqcallid, const std::string &_url, const std::string &_postdata);
	void onCallDone(HttpClientAsync::JobInfo _ji);
	void onRequesterFinished(int _reqid);
	
	void addRequester(RequesterPtr _requester);
	
	//void kick();
};

typedef boost::shared_ptr<HttpOutRequestDisp> HttpOutRequestDispPtr;

#endif
