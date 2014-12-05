#include "Parser.h"

namespace hiapi {

namespace client {

ResponseParser::ResponseParser():
m_got_response(false),
state(READING_HANDSHAKE),
m_handshake(HIAPI_HANDSHAKE) {
	
}

ResponseParser::~ResponseParser() {
	
}

void ResponseParser::parse() {
	
	//return;
	
	while (true) {
		
		if (m_cur_token.size() == 0)
			return;
	
		if (state == READING_HANDSHAKE) {
		
			if (m_cur_token.size() >= m_handshake.size()) {
			
				std::string handshake = m_cur_token.substr(0, m_handshake.size());
				m_cur_token = m_cur_token.substr(m_handshake.size(), m_cur_token.size() - m_handshake.size());
			
				if (handshake != m_handshake)
					throw CannotHandShakeEx();
				else {
					state = READING_SIZE;
					std::cout << "ResponseParser::parse HANDSHAKE OK!\n";
				}
			}
			else
				return;
		}
		
		if (state == READING_SIZE) {
			
			size_t endlpos = m_cur_token.find("\n");
			if (endlpos == std::string::npos)
				return;
			
			std::string size_str = m_cur_token.substr(0, m_cur_token.size() - endlpos);
			m_cur_token = m_cur_token.substr(size_str.size(), m_cur_token.size() - size_str.size());
			
			m_size = string_to_uint64(size_str);
			
			if (m_size > MESSAGE_SIZE_LIMIT)
				throw ResponseParsingEx();
			
			state = READING_MESSAGE;
		}
		
		if (state == READING_MESSAGE) {
			
			if (m_cur_token.size() >= m_size) {
				
				m_cur_response = m_cur_token.substr(0, m_size);
				m_got_response = true;
				m_cur_token = m_cur_token.substr(m_cur_response.size(), m_cur_token.size() - m_cur_response.size());
				state = READING_SIZE;
			}
			return;
		}
	}
}


bool ResponseParser::handshaked() {
	
	return state == READING_SIZE || state == READING_MESSAGE;
}

void ResponseParser::execute(const std::string &_d) {
	
	m_cur_token.append(_d);
	parse();
	
	//std::cout << "ResponseParser::execute " << m_cur_token << std::endl;
}

bool ResponseParser::hasResponse() {
	return m_got_response;
}

void ResponseParser::getResponse(std::string &_s) {
	
	m_got_response = false;
	_s = m_cur_response;
	parse();
}
	
}

}
