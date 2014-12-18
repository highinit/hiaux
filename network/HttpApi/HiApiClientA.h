#ifndef _HIAPI_CLIENT_A_
#define _HIAPI_CLIENT_A_

#include <map>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

/*

Application Client --> Api Client --> libcurl --> HTTP --> Http Server --> Api handler --> Application

(one connection per request, no keep-alive)

Application Client --> Api Client --> TCP --> Http Server (Upgrade to custom binary protocol) --> Api handler --> Application

(multiple persistent connections, binary lightweight protocol, server is still rest compatible)

*/

class HiApiClientA  {

public:

	HiApiClientA();
	virtual ~HiApiClientA();
	
	virtual void call (const std::string &_method, const std::map<std::string, std::string> &_params, const boost::function<void(bool, const std::string &)> &_onFinished) = 0;
	virtual void callSigned (const std::string &_method, const std::map<std::string, std::string> &_params, const boost::function<void(bool, const std::string &)> &_onFinished) = 0;
};

typedef boost::shared_ptr<HiApiClientA> HiApiClientAPtr;

#endif
