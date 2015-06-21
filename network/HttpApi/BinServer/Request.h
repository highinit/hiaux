#ifndef _API_BIN_REQUEST_
#define _API_BIN_REQUEST_

#include "hiaux/network/HttpServer/CustomRequest.h"
#include "hiaux/network/HttpApi/BinClient/Request.pb.h"
#include <boost/shared_ptr.hpp>

#include <map>

namespace hiapi {

namespace server {

class Request : public hiaux::CustomRequest {
public:
	
	std::string method;
	std::map<std::string, std::string> params;
	
	std::string dump;
	
	Request(const std::string &_dump);
	virtual ~Request();
};

typedef boost::shared_ptr<Request> RequestPtr;

}
}

#endif
