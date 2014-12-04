#ifndef _CUSTOM_REQUEST_H_
#define _CUSTOM_REQUEST_H_

#include <boost/shared_ptr.hpp>

class CustomRequest {

public:
	
	virtual ~CustomRequest();
};

typedef boost::shared_ptr<CustomRequest> CustomRequestPtr;

#endif
