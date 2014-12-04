#ifndef _API_BIN_PARSER_H_
#define _API_BIN_PARSER_H_

#include "../HttpServer/CustomParser.h"

class ApiBinParser : public CustomParser {

public:
	
	virtual void execute(const std::string &_d, bool &_request_finished);
	virtual CustomRequestPtr getRequest();
	virtual ~ApiBinParser();
};

typedef boost::shared_ptr<ApiBinParser> ApiBinParserPtr;

#endif
