
#include <sys/types.h>
#include <dirent.h>

#include <sys/uio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/uio.h> 
#include <unistd.h>

#include "../core/MRInterResult.h"
#include "../example_invindex/mapr_test.h"

class MaprTests
{
public:	
	
	void testInvLineDumper();
	void testMRInterResult();
};