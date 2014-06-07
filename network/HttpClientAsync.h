#ifndef _HTTP_CLIENT_ASYNC_H_
#define _HTTP_CLIENT_ASYNC_H_ 

#include "HttpClient.h"

#include <curl/curl.h>

#include <boost/function.hpp>
#include <string>

#include "hiaux/structs/hashtable.h"
#include "hiaux/threads/locks.h"

class HttpClientAsync {
public:
	class JobInfo {
	public:
		JobInfo(const std::string &_callid);
		std::string callid;
		std::string resp;
		bool success;
	};
	
private:
	hiaux::hashtable<CURL*, JobInfo> m_e_curls;
	
	CURLM *m_curl;
	boost::function<void(const std::string &_resp)> m_onCalled;
	int m_nrunning;
	
	hAutoLock lock;
	
	void performTransfers();
public:
	HttpClientAsync(boost::function<void(const std::string &_resp)> _onCalled);
	~HttpClientAsync();

	void call (const std::string &_callid, const std::string &_url);
	void kick();
};

#endif
