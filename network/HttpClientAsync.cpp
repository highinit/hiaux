#include "HttpClientAsync.h"


HttpClientAsync::JobInfo::JobInfo(void* _userdata, const std::string &_postdata):
success(false),
userdata(_userdata),
postdata(_postdata) {
	

}

/*
hLock g_curl_lock;

void lock_function(CURL *handle, curl_lock_data data, curl_lock_access access, void *userptr) {
	g_curl_lock.lock();
}

void unlock_function(CURL *handle, curl_lock_data data, void *userptr) {
	g_curl_lock.unlock();
}
*/

void _http_client_async_onFinished() {
	
}

HttpClientAsync::HttpClientAsync(boost::function<void(HttpClientAsync::JobInfo _ji)> _onCalled):
 m_onCalled(_onCalled) {
	 
//	 m_curl_sh = curl_share_init();
	 m_curl = curl_multi_init();
	 long timeout = 2000;
	 curl_multi_timeout(m_curl, &timeout);
	 
 	hThreadPoolPtr pool (new hThreadPool(2));
 	m_launcher = TaskLauncherPtr(new TaskLauncher(pool, 2, boost::bind(&_http_client_async_onFinished)));
 	pool->run();
	 
//	 curl_share_setopt(m_curl_sh, CURLSHOPT_LOCKFUNC, &lock_function);
//	 curl_share_setopt(m_curl_sh, CURLSHOPT_UNLOCKFUNC, &unlock_function);
	 
}

HttpClientAsync::~HttpClientAsync() {
	hLockTicketPtr ticket = lock.lock();
	/*
	hiaux::hashtable<CURL*, JobInfo>::iterator it = m_e_curls.begin();
	hiaux::hashtable<CURL*, JobInfo>::iterator end = m_e_curls.end();
	while (it != end) {
		curl_multi_remove_handle(m_curl, it->first);
		curl_easy_cleanup(it->first);
		it++;
	}
//	std::cout << "calling curl_multi_cleanup\n";
	curl_multi_cleanup(m_curl);
//	std::cout << "curl_multi_cleanup finished\n";
	*/
}

void HttpClientAsync::call (void* userdata, const std::string &_url) {
	
	hLockTicketPtr ticket = lock.lock();

	CURL *e_curl = curl_easy_init();

	m_e_curls.insert(std::pair<CURL*, JobInfo>(e_curl, JobInfo(userdata, "")));

	hiaux::hashtable<CURL*, JobInfo>::iterator it = m_e_curls.find(e_curl);
	
//	curl_easy_setopt(e_curl, CURLOPT_SHARE, m_curl_sh);
	curl_easy_setopt(e_curl, CURLOPT_URL, _url.c_str());
	curl_easy_setopt(e_curl, CURLOPT_WRITEFUNCTION, crawl_function_pt);
	curl_easy_setopt(e_curl, CURLOPT_WRITEDATA, &it->second.resp);
	curl_easy_setopt(e_curl, CURLOPT_ENCODING, "UTF-8");
	curl_easy_setopt(e_curl, CURLOPT_TIMEOUT, 5);
	curl_easy_setopt(e_curl, CURLOPT_USERAGENT, "hiaux HttpClient");

	curl_multi_add_handle(m_curl, e_curl);

}

void HttpClientAsync::callPost (void* userdata, const std::string &_url, const std::string &_postdata) {
	
	hLockTicketPtr ticket = lock.lock();

//	std::cout << "HttpClientAsync::callPost " << _postdata << std::endl;

	CURL *e_curl = curl_easy_init();

	m_e_curls.insert(std::pair<CURL*, JobInfo>(e_curl, JobInfo (userdata, _postdata)));
 
	hiaux::hashtable<CURL*, JobInfo>::iterator it = m_e_curls.find(e_curl);
	
//	curl_easy_setopt(e_curl, CURLOPT_SHARE, m_curl_sh);
	curl_easy_setopt(e_curl, CURLOPT_URL, _url.c_str());
	curl_easy_setopt(e_curl, CURLOPT_WRITEFUNCTION, crawl_function_pt);
	curl_easy_setopt(e_curl, CURLOPT_WRITEDATA, &it->second.resp);

	curl_easy_setopt(e_curl, CURLOPT_ENCODING, "UTF-8");
	curl_easy_setopt(e_curl, CURLOPT_CUSTOMREQUEST, "POST");

	curl_easy_setopt(e_curl, CURLOPT_POST, 1);
	curl_easy_setopt(e_curl, CURLOPT_TIMEOUT, 5);
	curl_easy_setopt(e_curl, CURLOPT_USERAGENT, "hiaux HttpClient");
	curl_easy_setopt(e_curl, CURLOPT_POSTFIELDS, it->second.postdata.data());
//	curl_easy_setopt(e_curl, CURLOPT_POSTFIELDS_SIZE, it->second.postdata.size()); 

	curl_multi_add_handle(m_curl, e_curl);
}

TaskLauncher::TaskRet HttpClientAsync::callHandler(JobInfo ji) {
	
	m_onCalled(ji);
	return TaskLauncher::NO_RELAUNCH;
}

void HttpClientAsync::performTransfers() {

	//hLockTicketPtr ticket = lock.lock();
//	while (curl_multi_perform(m_curl, &m_nrunning) == CURLM_CALL_MULTI_PERFORM) {
//		std::cout << "HttpClientAsync::performTransfers curl_multi_perform == CURLM_CALL_MULTI_PERFORM\n";
//	}
	
	int nmsg;
	CURLMsg* msg;// = curl_multi_info_read(m_curl, &nmsg);
	
	while ((msg = curl_multi_info_read(m_curl, &nmsg))) {
		if (msg->msg == CURLMSG_DONE) {
			
			hiaux::hashtable<CURL*, JobInfo>::iterator it = m_e_curls.find(msg->easy_handle);
			
			if (it != m_e_curls.end()) {
			
				if (msg->data.result == CURLE_OK)
					it->second.success = true;
				else {
					std::cout << "curlerror: " << curl_easy_strerror(msg->data.result) << std::endl;
					it->second.success = false;
				}
				JobInfo ji = it->second;
				curl_multi_remove_handle(m_curl, it->first);
				curl_easy_cleanup(it->first);
				m_e_curls.erase(it);
			

				//m_onCalled(ji);
				m_launcher->addTask(NEW_LAUNCHER_TASK3(&HttpClientAsync::callHandler, this, ji));
				
			}
		}
		//if (nmsg <= 0)
		//	break;
		//msg = curl_multi_info_read(m_curl, &nmsg);
	}
}

void HttpClientAsync::kick() {
	
	hLockTicketPtr ticket = select_lock.lock();
	
	int retval;
	int still_running;
	do {
		
		fd_set readfds;
		fd_set writefds;
		fd_set excfds;
	
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&excfds);
	
		int max_fd;
	
		curl_multi_fdset(m_curl, &readfds, &writefds, &excfds, &max_fd);
	
	    struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
	
	    retval = select(max_fd+1, &readfds, &writefds, &excfds, &tv);
		
		if (retval != -1)
			curl_multi_perform(m_curl, &still_running);
		
	} while (still_running);
	
	performTransfers();
}
