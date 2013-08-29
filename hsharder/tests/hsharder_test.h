#include <cxxtest/TestSuite.h>
#include "../hsharder.h"
#include <boost/crc.hpp>

int getCrc32(const std::string& my_string) 
{
    boost::crc_32_type result;
    result.process_bytes(my_string.data(), my_string.length());
    return result.checksum();
}

class HdividerDistTests : public CxxTest::TestSuite
{    
public:
    
    void testO()
    {
        boost::crc_32_type crc;
        
        hSharder<std::string, std::string> sharder(10, boost::bind(getCrc32, _1));  
        sharder.assignShardMapping(std::pair<int,int>(0, 5), std::string("_1_"));
        sharder.assignShardMapping(std::pair<int,int>(5, 10), std::string("_2_"));
        
        int s1 = 0, s2 = 0;
        for (int i = 0; i<1000; i++)
        {
            char bf[20];
            sprintf(bf, "%d", i);
            std::cout << bf << std::endl;
            /*std::string key(bf);
            if (sharder.getRealShard(key)=="_1_")
            {
                s1++;
            } else if (sharder.getRealShard(key)=="_2_")
            {
                s2++;
            } else
            {
                std::cout << "unknown shard\n";
                TS_ASSERT(0);
            }*/
          
        }
        
        std::cout << "s1:" << s1 << std::endl << "s2:" << s2 << std::endl;
    }
};