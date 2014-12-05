#ifndef _HIAPI_BINCLIENT_RESPONSE_PARSER_
#define _HIAPI_BINCLIENT_RESPONSE_PARSER_

#include "hiconfig.h"

#include "hiaux/strings/string_utils.h"

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <string>

#include "hiaux/network/HttpApi/HiApiConsts.h"

namespace hiapi {

namespace client {

class CannotHandShakeEx {
};

class ResponseParsingEx {
};


#define MESSAGE_SIZE_LIMIT 1024*1024 // 1Mb

class ResponseParser {
	
	enum State {
		READING_HANDSHAKE,
		READING_SIZE,
		READING_MESSAGE
	};
	
	State state;
	
	const std::string m_handshake;
	
	std::string m_cur_token;
	uint64_t m_size;
	
	std::string m_cur_response;
	bool m_got_response;
	
	
	void parse();
	
	boost::function<void()> m_onHandshaked;
	
public:
	
	ResponseParser(boost::function<void()> _onHandshaked);
	virtual ~ResponseParser();

	bool justHandshaked();
	
	void execute(const std::string &_d);
	bool hasResponse();
	void getResponse(std::string &_s);
};
	
}

}

#endif
