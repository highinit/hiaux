#include "HttpOutReqDisp.h"

HttpOutRequestDisp::OutRequestInfo::OutRequestInfo(int _reqid, int _requester_callid):
	 reqid(_reqid),
	 requester_callid(_requester_callid) {
	
}

HttpOutRequestDisp::Requester::Requester(boost::function<void(int, int, const std::string&)> _onCall,
			boost::function<void(int, int, const std::string&, const std::string&)> _onCallPost,
			boost::function<void(int)> _onFinished):
	m_onCall(_onCall),
	m_onCallPost(_onCallPost),
	m_onFinished(_onFinished),
	m_isfinished(false) {
				
}

void HttpOutRequestDisp::Requester::call (int _callid, const std::string &_url) {
	m_onCall(m_id, _callid, _url);
}

void HttpOutRequestDisp::Requester::callPost (int _callid, const std::string &_url, const std::string &_postdata) {
	m_onCallPost(m_id, _callid, _url, _postdata);
}

bool HttpOutRequestDisp::Requester::finished() {
	//m_onFinished(m_id);
	return m_isfinished;
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
		
	m_req_id = 0;
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

TaskLauncher::TaskRet HttpOutRequestDisp::onCallPostTask(int _reqid, int _reqcallid, const std::string &_url, const std::string &_postdata) {
	
	hLockTicketPtr ticket = lock.lock();
	m_http_client->callPost((void*)new OutRequestInfo(_reqid, _reqcallid), _url, _postdata);
	return TaskLauncher::NO_RELAUNCH;
}

void HttpOutRequestDisp::onCallPost(int _reqid, int _reqcallid, const std::string &_url, const std::string &_postdata) {
	
	m_launcher->addTask(NEW_LAUNCHER_TASK6 (&HttpOutRequestDisp::onCallPostTask, this, _reqid, _reqcallid, _url, _postdata) );
}

/////

TaskLauncher::TaskRet HttpOutRequestDisp::onCallDoneTask(HttpClientAsync::JobInfo _ji) {
	
	RequesterPtr requester;
	OutRequestInfo *reqinfo = (OutRequestInfo*)_ji.userdata;
	
	{
		hLockTicketPtr ticket = lock.lock();
	
		hiaux::hashtable<int, RequesterPtr>::iterator it = m_requesters.find(reqinfo->reqid);
	
		if (it != m_requesters.end()) {
			requester = it->second;
		} else
			std::cout << "HttpOutRequestDisp::onCallDone Requester " << reqinfo->reqid << " dont exists\n";
	}
	
	if (requester) {
		requester->onCallDone(reqinfo->requester_callid, _ji.success, _ji.resp);
		
		if (requester->finished())
			onRequesterFinished(requester->getId());
	}
	
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
	
	_req->setId(++m_req_id); //m_requesters.size());
	
//	std::cout << "HttpOutRequestDisp::addRequesterTask " << _req->getId() << std::endl;
	
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
	
//	std::cout << "HttpOutRequestDisp::onRequesterFinishedTask " << _reqid << std::endl;
	
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
	
	std::cout << "HttpOutRequestDisp::kickTask\n";
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
