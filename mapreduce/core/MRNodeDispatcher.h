#ifndef MRNODE_DISPATCHER
#define MRNODE_DISPATCHER

#include "../core/MRBatchDispatcher.h"
#include "../core/MRInterResult.h"

class InterResultQueue: public std::queue<MRInterResultPtr>, public hLock
{
	
};

class MRNodeDispatcher
{
	boost::shared_ptr<MRBatchDispatcher> batch_dispatcher;
	InterResultQueue inter_results;
	hThreadPool *pool;
	
	TaskLauncher reduce_tasks_launcher;
	TaskLauncher preload_tasks_launcher;
	
public:

	void reduceTask(MRInterResultPtr a, MRInterResultPtr b);
	void onGotResult(MRInterResultPtr inter_result);
};

#endif
