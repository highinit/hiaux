
#include "BinClientParserTests.h"

void BinClientParserTests::onHandshaked() {
	
	
}

BinClientParserTests::BinClientParserTests() {
	
	hiapi::client::ResponseParser parser(boost::bind(&BinClientParserTests::onHandshaked, this));
	
	std::ostringstream o;
	
	std::string mess = "x828324fslnljcnlasjdiwjef";
	
	o << HIAPI_HANDSHAKE << mess.size() << "\n" << mess << "14";
	
	std::string data (o.str());
	
	for (int i = 0; i<data.size(); i++)
		parser.execute( data.substr(i, 1) );
	
	TS_ASSERT(parser.hasResponse());
	
	std::string got;
	
	parser.getResponse(got);
	
	TS_ASSERT(got == mess);
}
