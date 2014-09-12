#include "HttpSimpleRequesterPost.h"

HttpSimpleRequesterPost::HttpSimpleRequesterPost(boost::function<void(int, int, const std::string&)> _onCall,
							boost::function<void(int, int, const std::string&, const std::string&)> _onCallPost,
							boost::function<void(int)> _onFinished,
							CallContextPtr _context,
							const std::string &_call_url,
							const std::string &_postdata,
							boost::function<void(CallContextPtr, const std::string&)> _onCalledOk,
							boost::function<void(CallContextPtr)> _onCalledFail):

		HttpOutRequestDisp::Requester(_onCall, _onCallPost, _onFinished),
		m_context(_context),
		m_call_url(_call_url),
		m_postdata(_postdata),
		m_onCalledOk(_onCalledOk),
		m_onCalledFail(_onCalledFail) {

}

void HttpSimpleRequesterPost::onCallDone (int _callid, bool _success, const std::string &_resp) {
	
	if (!_success) {
		m_onCalledFail(m_context);
		m_isfinished = true;
		///finished();
	}
	else {
		m_onCalledOk(m_context, _resp);
		m_isfinished = true;
		//finished();
	}
}

void HttpSimpleRequesterPost::start() {
		
	callPost(0, m_call_url, m_postdata);
}
