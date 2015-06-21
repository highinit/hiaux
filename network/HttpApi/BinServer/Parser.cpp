#include "Parser.h"

namespace hiapi {

namespace server {

Parser::Parser(hiaux::HttpRequestPtr _req):
state(READING_SIZE),
m_got_request(false) {
	
}

void Parser::parse() {
	
	while (true) {
		
		if (m_cur_token.size() == 0)
			return;
		
		if (state == READING_SIZE) {
			
			if (m_cur_token[0] == 'k') {
				
				if (m_cur_token.size() == 1) {
					
					m_cur_token.clear();
					return;
				} else {
					m_cur_token = m_cur_token.substr(1, m_cur_token.size() - 1);
					continue;
				}
			}
			
			size_t endlpos = m_cur_token.find("\n");
			if (endlpos == std::string::npos)
				return;
			
			std::string size_str = m_cur_token.substr(0, endlpos);
			m_cur_token = m_cur_token.substr(size_str.size() + 1, m_cur_token.size() - size_str.size() + 1);
			
			//std::cout << "Parser::parse size_str: " << size_str << " m_cur_token:" << m_cur_token << std::endl;
			
			m_size = string_to_uint64(size_str);
			
			if (m_size > HIAPI_BINREQUEST_MAX_SIZE)
				throw hiaux::RequestParsingEx();
			
			state = READING_MESSAGE;
		}
		
		if (state == READING_MESSAGE) {
			
			//std::cout << "state == READING_MESSAGE size: " << m_size << " got: " << m_cur_token.size() << std::endl;
			
			if (m_cur_token.size() >= m_size) {
				
				std::string cur_req_str = m_cur_token.substr(0, m_size);
				m_cur_request = RequestPtr(new Request(cur_req_str));
				
				m_got_request = true;
				m_cur_token = m_cur_token.substr(cur_req_str.size(), m_cur_token.size() - cur_req_str.size());
				state = READING_SIZE;
			}
			return;
		}
		
	}
}

void Parser::execute(const std::string &_d) {
	
	//return;
	m_cur_token.append(_d);
	//std::cout << "Parser::execute " << m_cur_token << std::endl;
	parse();
}

bool Parser::hasRequest() {
	
	return m_got_request;
}

hiaux::CustomRequestPtr Parser::getRequest() {
	
	m_got_request = false;
	RequestPtr req = m_cur_request;
	parse();
	return req;
}

Parser::~Parser() {
	
}


}
}
