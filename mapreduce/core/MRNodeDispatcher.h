#ifndef MRNODE_DISPATCHER
#define MRNODE_DISPATCHER

#include "../core/MRBatchDispatcher.h"
#include "../core/MRInterResult.h"

class InterResultQueue: public std::queue<MRInterResultPtr>, public hLock
{
	
};

class MRProgressBar
{
public:
	float map_p;
	float red_p;

	MRProgressBar()
	{
	
	}
	MRProgressBar(float m, float p):
		map_p(m),
		red_p(p)
	{
	}
};

class MRNodeDispatcher
{
	boost::shared_ptr<MRBatchDispatcher> batch_dispatcher;
	InterResultQueue inter_results;
	hThreadPool *pool;
	
	MapReduce *m_MR;
	
	std::atomic<size_t> nmerge;
	std::atomic<size_t> nbatches;
	
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
	
	std::string m_path;
	
	boost::function<void(MRInterResultPtr)> m_onFinished;
	uint64_t progress_bar_updated_ts;
	bool show_progress;
	boost::function<void(MRProgressBar)> m_showProgress;
	void CallProgressBar();
public:

	void onPreloadFinished();
	void onFlushFinished();
	void onReducesFinished();
	
	MRNodeDispatcher(hThreadPool *pool,
					MapReduce *MR,
					std::string path,
					boost::function<void(MRInterResultPtr)> onFinished,
					size_t nbatch_threads = 6,
					size_t nreduce_threads = 6,
					size_t npreaload_threads = 1,
					size_t nflush_threads = 1,
					size_t preload_buffer_size = 500000,
					size_t flush_buffer_size = 5000000);
	
	void setProgressBar(boost::function<void(MRProgressBar)> showProgress);
	
	bool reduceTask(MRInterResultPtr a, MRInterResultPtr b);
	void onAddResult(MRInterResultPtr inter_result); // from batcher
	void onGotResult(MRInterResultPtr inter_result); // from reduceTask
	void noMoreInter();
	void noMoreBatches();
	
	void addBatch(BatchAccessor* batch);
};

#endif
