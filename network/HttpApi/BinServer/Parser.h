#ifndef _API_BIN_PARSER_H_
#define _API_BIN_PARSER_H_

#include "hiaux/network/HttpServer/CustomParser.h"
#include "Request.h"


#define HIAPI_BINREQUEST_MAX_SIZE 1024*1024*3 // 3mb

namespace hiapi {

namespace server {

class Parser : public CustomParser {

	enum State {
		READING_SIZE,
		READING_MESSAGE
	};

	State state;
	
	std::string m_cur_token;
	uint64_t m_size;
	
	RequestPtr m_cur_request;
	bool m_got_request;
	
	void parse();

public:
	
	Parser(HttpRequestPtr _req);
	
	virtual void execute(const std::string &_d);
	virtual bool hasRequest();
	virtual CustomRequestPtr getRequest();
	virtual ~Parser();
};

typedef boost::shared_ptr<Parser> ParserPtr;

}
}

#endif
