#ifndef MRINTER_MERGER_H
#define MRINTER_MERGER_H

#include "MRInterResult.h"
#include "../../threadpool/taskcounter.h"

class MRInterMerger
{
public:
	
	static void merge(TaskLauncher &preload_tasks_launcher,
				MRInterResultPtr inter1,
				MRInterResultPtr inter2,
				MRInterResultPtr result,
				MapReduce *MR,
				int emits_in_cache);
	
	static void loadCache(MRInterResultPtr inter,
				bool cid,
				Int64VecPtr keys, int b, int e, hLock &lock);
	
	static Int64VecPtr mergeKeys(Int64VecPtr a, Int64VecPtr b);
};

#endif
