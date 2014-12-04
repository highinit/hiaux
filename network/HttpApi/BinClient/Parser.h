#ifndef _HIAPI_BINCLIENT_RESPONSE_PARSER_
#define _HIAPI_BINCLIENT_RESPONSE_PARSER_s

#include "hiconfig.h"

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <string>

namespace hiapi {

namespace client {

class ResponseParser {
public:

	void execute(const std::string &_d, bool &_got_response);
};
	
}

}

#endif
