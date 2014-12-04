#include "Request.h"

namespace hiapi {

namespace client {

Request::Request(const std::string &_data, const boost::function<void(bool, const std::string &)> &_onFinished):
data(_data),
onFinished(_onFinished) {
	
}

}

}

