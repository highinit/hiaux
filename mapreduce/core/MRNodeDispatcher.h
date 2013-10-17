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
	
	EmitDumper *m_dumper;
	MapReduce *m_MR;
	
	std::atomic<size_t> nmerge;
	
	TaskLauncher preload_tasks_launcher;
	TaskLauncher reduce_tasks_launcher;
	TaskLauncher flush_tasks_launcher;
	
	size_t m_nbatch_threads;
	size_t m_nreduce_threads;
	size_t m_npreaload_threads;
	size_t m_nflush_threads;
	size_t m_preload_buffer_size;
	size_t m_flush_buffer_size;
	
	MRStats m_stats;
	
	bool nomore_batches;
	bool nomore_inter;
	
public:

	void onPreloadFinished();
	void onFlushFinished();
	void onReducesFinished();
	
	MRNodeDispatcher(hThreadPool *pool,
					MapReduce *MR,
					EmitDumper *dumper,
					size_t nbatch_threads = 6,
					size_t nreduce_threads = 6,
					size_t npreaload_threads = 1,
					size_t nflush_threads = 1,
					size_t preload_buffer_size = 500000,
					size_t flush_buffer_size = 500000);
	
	bool reduceTask(MRInterResultPtr a, MRInterResultPtr b);
	void onAddResult(MRInterResultPtr inter_result); // from batcher
	void onGotResult(MRInterResultPtr inter_result); // from reduceTask
	void noMoreInter();
	void noMoreBatches();
	
	void addBatch(BatchAccessor* batch);
};

#endif
