
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
	hLock load_lock;
public:	
	
	void testInvLineDumper();
	void testMRInterResult();
	
	void loadCache(MRInterResult *inter,
				bool cid,
				Int64VecPtr keys, int b, int e);
	void testMRInterResultAsync();
	
	void onMRInterMergerFinished();
	void testMRInterMerger();
};
