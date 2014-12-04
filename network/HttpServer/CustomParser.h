#ifndef _CUSTOM_PARSER_H_
#define _CUSTOM_PARSER_H_

#include "CustomRequest.h"
#include <string>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

class CustomParser {
	
public:

	virtual void execute(const std::string &_d, bool &_request_finished) = 0;
	virtual CustomRequestPtr getRequest() = 0;
	virtual ~CustomParser();
};

typedef boost::shared_ptr<CustomParser> CustomParserPtr;

#endif
