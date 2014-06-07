#include "HttpClientAsync.h"

HttpClientAsync::JobInfo::JobInfo(const std::string &_callid):
success(false),
callid(_callid) {
	
}

HttpClientAsync::HttpClientAsync(boost::function<void(const std::string &_resp)> _onCalled):
 m_curl(curl_multi_init()),
 m_onCalled(_onCalled) {

}

void HttpClientAsync::call (const std::string &_callid, const std::string &_url) {
	
	hLockTicketPtr ticket = lock.lock();
	
	CURL *e_curl = curl_easy_init();
	
	m_e_curls.insert(std::pair<CURL*, JobInfo>(e_curl, JobInfo(_callid)));
	
	hiaux::hashtable<CURL*, JobInfo>::iterator it = m_e_curls.find(e_curl);
	
	curl_easy_setopt(e_curl, CURLOPT_URL, _url.c_str());
	curl_easy_setopt(e_curl, CURLOPT_WRITEFUNCTION, crawl_function_pt);
	curl_easy_setopt(e_curl, CURLOPT_WRITEDATA, &it->second.resp);
	curl_easy_setopt(e_curl, CURLOPT_ENCODING, "UTF-8");
	curl_easy_setopt(e_curl, CURLOPT_TIMEOUT, 5);
	curl_easy_setopt(e_curl, CURLOPT_USERAGENT, "hiaux HttpClient");
	
	curl_multi_add_handle(m_curl, e_curl);
}

void HttpClientAsync::performTransfers() {
	curl_multi_perform(m_curl, &m_nrunning);
	
	int nmsg;
	CURLMsg* msg = curl_multi_info_read(m_curl, &nmsg);
	
	if (msg != NULL) 
		if (msg->msg == CURLMSG_DONE) {
			hiaux::hashtable<CURL*, JobInfo>::iterator it = m_e_curls.find(msg->easy_handle);
		}
	
}

void HttpClientAsync::kick() {
	
	hLockTicketPtr ticket = lock.lock();
	performTransfers();
}

HttpClientAsync::~HttpClientAsync() {
	curl_multi_cleanup(m_curl);
}
