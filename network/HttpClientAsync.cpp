#include "HttpClientAsync.h"

HttpClientAsync::JobInfo::JobInfo(void* _userdata):
success(false),
userdata(_userdata) {
	
}

HttpClientAsync::HttpClientAsync(boost::function<void(HttpClientAsync::JobInfo _ji)> _onCalled):
 m_curl(curl_multi_init()),
 m_onCalled(_onCalled) {

}

HttpClientAsync::~HttpClientAsync() {
	hLockTicketPtr ticket = lock.lock();
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
}

void HttpClientAsync::call (void* userdata, const std::string &_url) {
	
	hLockTicketPtr ticket = lock.lock();
	
	CURL *e_curl = curl_easy_init();
	
	m_e_curls.insert(std::pair<CURL*, JobInfo>(e_curl, JobInfo(userdata)));
	
	hiaux::hashtable<CURL*, JobInfo>::iterator it = m_e_curls.find(e_curl);
	
	curl_easy_setopt(e_curl, CURLOPT_URL, _url.c_str());
	curl_easy_setopt(e_curl, CURLOPT_WRITEFUNCTION, crawl_function_pt);
	curl_easy_setopt(e_curl, CURLOPT_WRITEDATA, &it->second.resp);
	curl_easy_setopt(e_curl, CURLOPT_ENCODING, "UTF-8");
	curl_easy_setopt(e_curl, CURLOPT_TIMEOUT, 5);
	curl_easy_setopt(e_curl, CURLOPT_USERAGENT, "hiaux HttpClient");
	
	curl_multi_add_handle(m_curl, e_curl);
	
	performTransfers();
}

void HttpClientAsync::performTransfers() {

	curl_multi_perform(m_curl, &m_nrunning);
	
	int nmsg;
	CURLMsg* msg = curl_multi_info_read(m_curl, &nmsg);
	
	while (msg != NULL) {
		if (msg->msg == CURLMSG_DONE) {
			hiaux::hashtable<CURL*, JobInfo>::iterator it = m_e_curls.find(msg->easy_handle);
			
			if (it != m_e_curls.end()) {
			
				if (msg->data.result == CURLE_OK)
					it->second.success = true;
				else
					it->second.success = false;
			
				JobInfo ji = it->second;
				curl_multi_remove_handle(m_curl, it->first);
				curl_easy_cleanup(it->first);
				m_e_curls.erase(it);
			
				m_onCalled(ji);
			}
		}
		if (nmsg == 0)
			break;
		msg = curl_multi_info_read(m_curl, &nmsg);
	}
}

void HttpClientAsync::kick() {
	
	hLockTicketPtr ticket = lock.tryLock();
	if (ticket)
		performTransfers();
}

