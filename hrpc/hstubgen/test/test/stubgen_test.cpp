#include "gtest/gtest.h"
#include "call_test.h"
#include "fault_test.h"
#include "../../src/stubgen.h"




//#include "commonstub_test.h"
//#include "stubgen_test.h"

//#pragma comment(lib, "gtestd.lib")

int subi = 0;
//bool stubs = 0;
int tabs = 0;

int md()
{
	
	return 0;
}

TEST(asd, md)
{
	
	
}

int main(int argc, char *argv[])
{
/*	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(1,1), &wsaData))
		{
		throw "hsock_t::server: Wsastartup fail";;
		return 0;
		}
*/
	::testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();

return 0;
}
