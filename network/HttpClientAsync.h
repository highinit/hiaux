#ifndef _HTTP_CLIENT_ASYNC_H_
#define _HTTP_CLIENT_ASYNC_H_ 

#include "HttpClient.h"

#include <curl/curl.h>

#include <boost/function.hpp>
#include <string>

#include "hiaux/structs/hashtable.h"
#include "hiaux/threads/locks.h"

#include <sys/select.h>

class HttpClientAsync {
public:
	class JobInfo {
	public:
		JobInfo(void* _userdata, const std::string &_postdata);
		void* userdata;
		std::string resp;
		bool success;
		std::string postdata;
	};
	
private:
	hiaux::hashtable<CURL*, JobInfo> m_e_curls;
	
	CURLM *m_curl;
	//CURLSH *m_curl_sh;
	boost::function<void(HttpClientAsync::JobInfo _ji)> m_onCalled;
	int m_nrunning;
	
	hAutoLock lock;
	hAutoLock select_lock;
	
	void performTransfers();
public:
	HttpClientAsync(boost::function<void(HttpClientAsync::JobInfo _ji)> _onCalled);
	~HttpClientAsync();

	void call (void* userdata, const std::string &_url);
	void callPost (void* userdata, const std::string &_url, const std::string &_postdata);
	void kick();
};

typedef boost::shared_ptr<HttpClientAsync> HttpClientAsyncPtr;

#endif
