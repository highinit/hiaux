#ifndef _API_BIN_REQUEST_
#define _API_BIN_REQUEST_

#include "hiaux/network/HttpServer/CustomRequest.h"
#include <boost/shared_ptr.hpp>

namespace hiapi {

namespace server {

class Request : public CustomRequest {
public:
	
	Request(const std::string &_dump);
	virtual ~Request();
};

typedef boost::shared_ptr<Request> RequestPtr;

}
}

#endif
