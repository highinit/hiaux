#ifndef _HIAPI_CLIENT_A_
#define _HIAPI_CLIENT_A_

#include <map>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

/*

Application Client --> Api Client --> libcurl --> HTTP --> Http Server --> Api handler --> Application

(one connection per request, no keep-alive)

Application Client --> Api Client --> TCP --> Http Server (Upgrade to custom binary protocol) --> Api handler --> Application

(multiple persistent connections, binary lightweight protocol, server is still rest compatible)

*/

class HiApiClientA : public boost::noncopyable {

public:
	enum Mode {
		HTTP,
		BINARY,
		LOCALSOCKET
	};

private:
	Mode m_mode;
	std::string m_user;
	std::string m_key;
	bool m_keyset;

	std::string m_endpoint;

public:
	
	HiApiClientA(Mode _mode, const std::string &_endpoint);
	HiApiClientA(Mode _mode, const std::string &_endpoint, const std::string &_user, const std::string &_key);
	virtual ~HiApiClientA();
	
	void call (const std::string &_method, std::map<std::string, std::string> &_params, boost::function<void(bool, const std::string &)> &_onFinished);
	void callSigned (const std::string &_method, std::map<std::string, std::string> &_params, boost::function<void(bool, const std::string &)> &_onFinished);
};

#endif
