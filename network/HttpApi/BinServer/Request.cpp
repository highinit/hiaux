#include "Request.h"

namespace hiapi {

namespace server {

Request::Request(const std::string &_dump):
dump(_dump) {
	
	hiapi_client::RequestPb pb;
	pb.ParseFromString(_dump);
	
	method = pb.method();
	for (int i = 0; i<pb.params_size(); i++) {
		
		const hiapi_client::RequestParamPb p = pb.params(i);
		params[p.key()] = p.value();
	}
}

Request::~Request() {
	
}

}
}
