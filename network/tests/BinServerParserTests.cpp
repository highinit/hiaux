#include "BinServerParserTests.h"

BinServerParserTests::BinServerParserTests() {
	
	hiapi::server::ParserPtr parser(new hiapi::server::Parser(HttpRequestPtr()));
	
	for (int i = 0; i<100; i++) {
	
		std::map<std::string, std::string> params;
		
		for (int j = 0; j<i+1; j++) {
		
			std::ostringstream ko;
			std::ostringstream vo;
		
			ko << "key" << j;
			vo << "value" << j;
		
			params[ko.str()] = vo.str();
		}
		
		std::string req_dump;
		
		hiapi::client::BinClientA::buildRequest("test_method", params, req_dump);
	
		std::ostringstream o;
		o << req_dump.size() << "\n" << req_dump;
	
		parser->execute(o.str());
	
		TS_ASSERT(parser->hasRequest());
	
		CustomRequestPtr cus_req = parser->getRequest();
	
		hiapi::server::Request *req = dynamic_cast<hiapi::server::Request*>( cus_req.get() );
	
		TS_ASSERT(req->method == "test_method");
	
		std::map<std::string, std::string>::iterator it = params.begin();
		std::map<std::string, std::string>::iterator end = params.end();
	
		while (it != end) {
		
			TS_ASSERT(it->second == req->params[it->first]);
		
			it++;
		}
	}
}
