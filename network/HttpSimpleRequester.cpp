#include "HttpSimpleRequester.h"

HttpSimpleRequester::HttpSimpleRequester(boost::function<void(int, int, const std::string&)> _onCall,
							boost::function<void(int)> _onFinished,
							CallContextPtr _context,
							const std::string &_call_url,
							boost::function<void(CallContextPtr, const std::string&)> _onCalledOk,
							boost::function<void(CallContextPtr)> _onCalledFail):

		HttpOutRequestDisp::Requester(_onCall, _onFinished),
		m_context(_context),
		m_call_url(_call_url),
		m_onCalledOk(_onCalledOk),
		m_onCalledFail(_onCalledFail) {

}

void HttpSimpleRequester::onCallDone (int _callid, bool _success, const std::string &_resp) {
	
	if (!_success) {
		m_onCalledFail(m_context);
		finished();
	}
	else {
		m_onCalledOk(m_context, _resp);
		finished();
	}
}

void HttpSimpleRequester::start() {
		
	call(0, m_call_url);
}
