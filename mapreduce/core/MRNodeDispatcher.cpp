#include "MRNodeDispatcher.h"
#include "MRInterMerger.h"

void MRNodeDispatcher::onPreloadFinished()
{
	std::cout << "MRNodeDispatcher::onPreloadFinished \n";
	exit(0);
}

void MRNodeDispatcher::onFlushFinished()
{
	std::cout << "MRNodeDispatcher::onFlushFinished \n";
	exit(0);
}
void MRNodeDispatcher::onReducesFinished()
{
	std::cout << "REDUCING FINISHED\n";
	
	inter_results.lock();
	std::cout << "result.size() " << inter_results.size() << std::endl;
	MRInterResultPtr result = inter_results.front();
	inter_results.pop();
	inter_results.unlock();
	
	//std::cout << "MRNodeDispatcher::onReducesFinished result->waitFlushFinished ";
	//result->waitFlushFinished();
	//std::cout << "OK\n";
	
	m_stats += batch_dispatcher->getStats();
	std::cout << "maps: " << m_stats.nmaps << std::endl;
	std::cout << "emits: " << m_stats.nemits << std::endl;
	std::cout << "reduces: " << m_stats.nreduces << std::endl;

	m_onFinished();
}

MRNodeDispatcher::MRNodeDispatcher(hThreadPool *pool,
									MapReduce *MR,
									std::string path,
									boost::function<void()> onFinished,
									size_t nbatch_threads,
									size_t nreduce_threads,
									size_t npreaload_threads,
									size_t nflush_threads,
									size_t preload_buffer_size,
									size_t flush_buffer_size):
	nmerge(0),
	m_MR(MR),
	m_preload_buffer_size(preload_buffer_size),
	m_flush_buffer_size(flush_buffer_size),
	preload_tasks_launcher(pool, npreaload_threads, boost::bind(&MRNodeDispatcher::onPreloadFinished, this)),
	reduce_tasks_launcher(pool, nreduce_threads, boost::bind(&MRNodeDispatcher::onReducesFinished, this)),
	flush_tasks_launcher(pool, nflush_threads, boost::bind(&MRNodeDispatcher::onFlushFinished, this)),
	nomore_inter(0),
	nomore_batches(0),
	m_path(path),
	m_onFinished(onFinished),
	batch_dispatcher (new MRBatchDispatcher(MR,
											pool,
											nbatch_threads,
											flush_tasks_launcher,
											path,
											boost::bind(&MRNodeDispatcher::onAddResult, this, _1),
											boost::bind(&MRNodeDispatcher::noMoreInter, this)
					)
	)
{
	
}

bool MRNodeDispatcher::reduceTask(MRInterResultPtr a, MRInterResultPtr b)
{
	char filename[50];
	sprintf(filename, "merge%d", (int)nmerge.fetch_add(1));
	MRInterResultPtr result ( new MRInterResult(m_path+filename,
												m_MR,
												flush_tasks_launcher,
												m_flush_buffer_size));
	
	//a->waitFlushFinished();
	//b->waitFlushFinished();
	m_stats += MRInterMerger::merge(preload_tasks_launcher,
									a,
									b,
									result,
									m_MR,
									m_preload_buffer_size);
	
	result->waitFlushFinished();
	onGotResult(result);
	return 0;
}

void MRNodeDispatcher::onAddResult(MRInterResultPtr inter_result)
{
	inter_result->waitFlushFinished();
	//std::cout << "MRNodeDispatcher::onAddResultk inter_results.lock() ";
	inter_results.lock();
	//std::cout << "OK\n";
	//std::cout << "MRNodeDispatcher::onAddResult inter_result->waitFlushFinished() ";
	//inter_result->waitFlushFinished();
	//std::cout << "OK\n";
	
	inter_results.push(inter_result);
	inter_results.unlock();
}

void MRNodeDispatcher::onGotResult(MRInterResultPtr inter_result)
{
//	std::cout << "MRNodeDispatcher::onGotResult inter_results.lock ";
	inter_results.lock();
//	std::cout << " OK\n";
	inter_results.push(inter_result);
		
	if (inter_results.size()==1 && nomore_inter && reduce_tasks_launcher.countRunning()<=1)
	{
		reduce_tasks_launcher.setNoMoreTasks();
	}
	
	while (inter_results.size()>=2)
	{
		MRInterResultPtr a = inter_results.front();
		inter_results.pop();
		MRInterResultPtr b = inter_results.front();
		inter_results.pop();
		
		reduce_tasks_launcher.addTask(new boost::function<bool()>
				(boost::bind(&MRNodeDispatcher::reduceTask, this, a, b)));
	}
		
/*	if (inter_results.size() <= 1 && nomore_inter)
	{
		std::cout << "SETTING reduce_tasks_launcher.setNoMoreTasks()\n";
		reduce_tasks_launcher.setNoMoreTasks();
	}*/
	
	inter_results.unlock();
}

void MRNodeDispatcher::noMoreInter()
{
	std::cout << "Batching finished, reducing..\n";
	
	// run reduces
//	std::cout << "MRNodeDispatcher::noMoreInter inter_results.lock ";
	inter_results.lock();
//	std::cout << " OK\n";
	
	nomore_inter = 1;
	
	while (inter_results.size()>=2)
	{
		MRInterResultPtr a = inter_results.front();
		inter_results.pop();
		MRInterResultPtr b = inter_results.front();
		inter_results.pop();
		
		reduce_tasks_launcher.addTask(new boost::function<bool()>
				(boost::bind(&MRNodeDispatcher::reduceTask, this, a, b)));
	}
	
	inter_results.unlock();
}

void MRNodeDispatcher::noMoreBatches()
{
	std::cout << "no more batches\n";
	batch_dispatcher->noMore();
}

void MRNodeDispatcher::addBatch(BatchAccessor* batch)
{
	batch_dispatcher->addBatch(batch);
}
