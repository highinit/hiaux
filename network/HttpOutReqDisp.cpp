#include "HttpOutReqDisp.h"

HttpOutRequestDisp::OutRequestInfo::OutRequestInfo(int _reqid, int _requester_callid):
	 reqid(_reqid),
	 requester_callid(_requester_callid) {
	
}

HttpOutRequestDisp::Requester::Requester(boost::function<void(int, int, const std::string&)> _onCall,
			boost::function<void(int)> _onFinished,
			int id):
	m_onCall(_onCall),
	m_onFinished(_onFinished),
	m_id(id) {
				
}

void HttpOutRequestDisp::Requester::call (int _callid, const std::string &_url) {
	m_onCall(m_id, _callid, _url);
}

void HttpOutRequestDisp::Requester::finished() {
	m_onFinished(m_id);
}

int HttpOutRequestDisp::Requester::getId() {
	return m_id;
}

HttpOutRequestDisp::HttpOutRequestDisp(TaskLauncherPtr _launcher):
	 m_http_client(new HttpClientAsync(boost::bind(&HttpOutRequestDisp::onCallDone, this, _1))),
	 m_launcher(_launcher) {
	
}

void HttpOutRequestDisp::onCall(int _reqid, int _reqcallid, const std::string &_url) {
	m_http_client->call((void*)new OutRequestInfo(_reqid, _reqcallid), _url);
}

TaskLauncher::TaskRet HttpOutRequestDisp::callDoneNotify(HttpOutRequestDisp::RequesterPtr _req, int _callid, bool _success, std::string _resp) {
	_req->onCallDone(_callid, _success, _resp);
	return TaskLauncher::NO_RELAUNCH;
}

TaskLauncher::TaskRet HttpOutRequestDisp::callStart(HttpOutRequestDisp::RequesterPtr _req) {
	_req->start();
	return TaskLauncher::NO_RELAUNCH;
}

void HttpOutRequestDisp::onCallDone(HttpClientAsync::JobInfo _ji) {
	
	hLockTicketPtr ticket = lock.lock();
	
	OutRequestInfo *reqinfo = (OutRequestInfo*)_ji.userdata;
	
	hiaux::hashtable<int, RequesterPtr>::iterator it = m_requesters.find(reqinfo->reqid);
	if (it != m_requesters.end()) {
		//it->second->onCallDone(reqinfo->requester_callid, _ji.success, _ji.resp);
		m_launcher->addTask(NEW_LAUNCHER_TASK6 (&HttpOutRequestDisp::callDoneNotify,
												this,
												it->second,
												reqinfo->requester_callid,
												_ji.success,
												_ji.resp) );
	} 
	else {
		std::cout << "Punkt::onCallDone Requester dont exists\n";
	}
	delete reqinfo;
	
	//m_http_client->kick();
}

void HttpOutRequestDisp::onRequesterFinished(int _reqid) {
	
	hLockTicketPtr ticket = lock.lock();
	
	hiaux::hashtable<int, RequesterPtr>::iterator it = m_requesters.find(_reqid);
	if (it != m_requesters.end()) {
		m_requesters.erase(it);
	} else {
		std::cout << "Punkt::onRequesterFinished Requester dont exists\n";
	}
	
	//m_http_client->kick();
}

void HttpOutRequestDisp::addRequester(RequesterPtr _requester) {
	
	hLockTicketPtr ticket = lock.lock();
	
	m_requesters.insert(std::pair<int, RequesterPtr> (_requester->getId(), _requester));
	
	m_launcher->addTask( NEW_LAUNCHER_TASK3 (&HttpOutRequestDisp::callStart, this, _requester));
	
	//_requester->start();
	//m_http_client->kick();
}

void HttpOutRequestDisp::kick() {
	m_http_client->kick();
}