
#include <sys/types.h>
#include <dirent.h>

#include <sys/uio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/uio.h> 
#include <unistd.h>

#include "../core/MRInterResult.h"
#include "../example_invindex/mapr_test.h"
#include "../core/MRBatchDispatcher.h"

class MaprTests
{
	hLock load_lock;
	
	MRBatchDispatcher *mr_disp;
public:	
	std::string m_path;
	
	void testInvLineDumper();
	void testMRInterResult();
	
	bool loadCache(MRInterResult *inter,
				bool cid,
				Int64VecPtr keys, int b, int e);
	void testMRInterResultAsync();
	
	void onMRInterMergerFinished();
	void testMRInterMerger();
	
	void onGotResult(MRInterResultPtr res);
	void onBatchingFinished();
	void testBatcher();
	
	void testNodeDispatcher();
};
