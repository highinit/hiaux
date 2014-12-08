
#include "BinClientParserTests.h"

void BinClientParserTests::onHandshaked() {
	
	
}

BinClientParserTests::BinClientParserTests() {
	
	hiapi::client::ResponseParser parser(boost::bind(&BinClientParserTests::onHandshaked, this));
	
	{
		std::ostringstream o;
		o << HIAPI_HANDSHAKE;
		parser.execute(o.str());
	}
	
	for (int i = 0; i<1000; i++) {
	
		std::ostringstream o;
	
		std::string mess;
		
		{
			std::ostringstream mess_o;
			for (int j = 0; j<=i; j++)
				mess_o << "a";
			mess = mess_o.str();
	 	}
		
		o << mess.size() << "\n" << mess;
	
		std::string data (o.str());
	
		for (int i = 0; i<data.size(); i++)
			parser.execute( data.substr(i, 1) );
	
		//parser.execute(data);
	
		TS_ASSERT(parser.hasResponse());
	
		std::string got;
	
		parser.getResponse(got);
		
		//std::cout << "mess: " << mess << std::endl; 
		//std::cout << "got: " << got << std::endl;
		
		
		TS_ASSERT(got == mess);
	}
}
