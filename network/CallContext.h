#ifndef _CALL_CONTEXT_
#define _CALL_CONTEXT_

#include "hiconfig.h"
#include <boost/shared_ptr.hpp>

class CallContext {

public:
	virtual ~CallContext() { }
};

typedef boost::shared_ptr<CallContext> CallContextPtr;

#endif
