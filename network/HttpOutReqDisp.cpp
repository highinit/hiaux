#include "HttpOutReqDisp.h"

HttpOutRequestDisp::OutRequestInfo::OutRequestInfo(int _reqid, int _requester_callid):
	 reqid(_reqid),
	 requester_callid(_requester_callid) {
	
}

HttpOutRequestDisp::Requester::Requester(boost::function<void(int, int, const std::string&)> _onCall,
			boost::function<void(int)> _onFinished):
	m_onCall(_onCall),
	m_onFinished(_onFinished) {
				
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

void HttpOutRequestDisp::Requester::setId(int _id) {
	m_id = _id;
}

HttpOutRequestDisp::HttpOutRequestDisp(TaskLauncherPtr _launcher):
	m_http_client(new HttpClientAsync(boost::bind(&HttpOutRequestDisp::onCallDone, this, _1))),
	m_launcher(_launcher),
	cond_kicking(boost::bind(&HttpOutRequestDisp::isKickStopped, this)) {

	kick_running = true;
	kick_stopped = false;
	_launcher->addTask( NEW_LAUNCHER_TASK2 (&HttpOutRequestDisp::kickTask, this));
}

bool HttpOutRequestDisp::isKickStopped() {
	
	return kick_stopped;
}

HttpOutRequestDisp::~HttpOutRequestDisp() {
	
	kick_running = false;
	
	cond_kicking.wait();
}

TaskLauncher::TaskRet HttpOutRequestDisp::onCallTask(int _reqid, int _reqcallid, const std::string &_url) {
	
	hLockTicketPtr ticket = lock.lock();
	m_http_client->call((void*)new OutRequestInfo(_reqid, _reqcallid), _url);
	return TaskLauncher::NO_RELAUNCH;
}

void HttpOutRequestDisp::onCall(int _reqid, int _reqcallid, const std::string &_url) {
	
	m_launcher->addTask(NEW_LAUNCHER_TASK5 (&HttpOutRequestDisp::onCallTask, this, _reqid, _reqcallid, _url) );
}

/////

TaskLauncher::TaskRet HttpOutRequestDisp::onCallDoneTask(HttpClientAsync::JobInfo _ji) {
	
	hLockTicketPtr ticket = lock.lock();
	
	OutRequestInfo *reqinfo = (OutRequestInfo*)_ji.userdata;
	
	hiaux::hashtable<int, RequesterPtr>::iterator it = m_requesters.find(reqinfo->reqid);
	
	if (it != m_requesters.end()) {
		it->second->onCallDone(reqinfo->requester_callid, _ji.success, _ji.resp);
	} else
		std::cout << "HttpOutRequestDisp::onCallDone Requester dont exists\n";
	
	delete reqinfo;
	
	return TaskLauncher::NO_RELAUNCH;
}

void HttpOutRequestDisp::onCallDone(HttpClientAsync::JobInfo _ji) {
	
	m_launcher->addTask(NEW_LAUNCHER_TASK3(&HttpOutRequestDisp::onCallDoneTask,
												this,
												_ji));

}

/////

TaskLauncher::TaskRet HttpOutRequestDisp::addRequesterTask(HttpOutRequestDisp::RequesterPtr _req) {
	
	hLockTicketPtr ticket = lock.lock();
	
	_req->setId(m_requesters.size());
	
	m_requesters.insert(std::pair<int, RequesterPtr> (_req->getId(), _req));
	_req->start();
	
	return TaskLauncher::NO_RELAUNCH;
}

void HttpOutRequestDisp::addRequester(RequesterPtr _requester) {
	
	m_launcher->addTask( NEW_LAUNCHER_TASK3 (&HttpOutRequestDisp::addRequesterTask, this, _requester));
}

/////

TaskLauncher::TaskRet HttpOutRequestDisp::onRequesterFinishedTask(int _reqid) {
	
	hLockTicketPtr ticket = lock.lock();
	
	hiaux::hashtable<int, RequesterPtr>::iterator it = m_requesters.find(_reqid);
	if (it != m_requesters.end()) {
		m_requesters.erase(it);
	} else {
		std::cout << "HttpOutRequestDisp::onRequesterFinished Requester dont exists\n";
	}
	return TaskLauncher::NO_RELAUNCH;
}

void HttpOutRequestDisp::onRequesterFinished(int _reqid) {
		
	m_launcher->addTask(NEW_LAUNCHER_TASK3 (&HttpOutRequestDisp::onRequesterFinishedTask, this, _reqid));
}

TaskLauncher::TaskRet HttpOutRequestDisp::kickTask() {
	
	while (kick_running)
		m_http_client->kick();
	
	cond_kicking.lock();
	kick_stopped = true;
	cond_kicking.unlock();
	
	cond_kicking.kick();
	return TaskLauncher::NO_RELAUNCH;
}

/*
void HttpOutRequestDisp::kick() {
	
	hLockTicketPtr ticket = kicklock.tryLock();
	if (ticket)
		m_launcher->addTask( NEW_LAUNCHER_TASK2 (&HttpOutRequestDisp::callKick, this));
}*/
