#ifndef _HIAPI_BINCLIENT_REQUEST_
#define _HIAPI_BINCLIENT_REQUEST_

#include "hiconfig.h"

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <string>

namespace hiapi {

namespace client {
	
class Request {
public:
	std::string data;
	boost::function<void(bool, const std::string &)> onFinished;

	Request(const std::string &_data, const boost::function<void(bool, const std::string &)> &_onFinished);
};

typedef boost::shared_ptr<Request> RequestPtr;
	
}

}

#endif
