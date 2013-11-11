#include "MRNodeDispatcher.h"
#include "MRInterMerger.h"
#include <iomanip>

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
	
	result->waitInitReading();
	CallProgressBar();
	//std::cout << "MRNodeDispatcher::onReducesFinished result->waitFlushFinished ";
	//result->waitFlushFinished();
	//std::cout << "OK\n";
	
	m_stats += batch_dispatcher->getStats();
	std::cout << "maps: " << m_stats.nmaps << std::endl;
	std::cout << "emits: " << m_stats.nemits << std::endl;
	std::cout << "reduces: " << m_stats.nreduces << std::endl;

	m_onFinished(result);
}

MRNodeDispatcher::MRNodeDispatcher(hThreadPool *pool,
									MapReduce *MR,
									std::string path,
									boost::function<void(MRInterResultPtr)> onFinished,
									size_t nbatch_threads,
									size_t nreduce_threads,
									size_t npreaload_threads,
									size_t nflush_threads,
									size_t preload_buffer_size,
									size_t flush_buffer_size):
	progress_bar_updated_ts(0),
	show_progress(0),
	nmerge(0),
	nbatches(0),
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

void MRNodeDispatcher::setProgressBar(boost::function<void(MRProgressBar)> showProgress)
{
	m_showProgress = showProgress;
	show_progress = 1;
}

void MRNodeDispatcher::CallProgressBar()
{
	if (!show_progress) return;
	uint64_t now = time(0);
	
	if (now-progress_bar_updated_ts < 1)
		return;
	
	progress_bar_updated_ts = now;
	
	float red_p = 0;
	
	uint32_t ngens = log2((double)nbatches);
	float ngen = 0;
	float mer_in_gen = (float)nbatches;
	float mer_before = 0;
	size_t nmerges = nmerge.load();
	while (mer_in_gen>=1 && nmerges > mer_before)
	{
		ngen++;
		mer_in_gen /= 2;
		mer_before += mer_in_gen;
	}
	
	MRProgressBar bar;
	bar.map_p = batch_dispatcher->getFinishPercentage();
	if (bar.map_p<=99.999f)
	{
		bar.red_p = 0;
	}
	else
	{
		bar.red_p = (100* (ngen-1 + (nmerges-mer_before+mer_in_gen)/mer_in_gen) / ngens);
	}
	if (bar.red_p > 100) 
		bar.red_p = 100;
	
	m_showProgress(bar);
	
	//std::cout << "REDUCE PROGRESS: " << setiosflags(std::ios::fixed) <<
	//		std::setprecision(1) << bar.red_p << std::endl;
}

bool MRNodeDispatcher::reduceTask(MRInterResultPtr a, MRInterResultPtr b)
{
	char filename[50];
	sprintf(filename, "merge%d", (int)nmerge.fetch_add(1));
	MRInterResultPtr result ( new MRInterResult(m_path+filename,
												m_MR,
												flush_tasks_launcher,
												m_flush_buffer_size));
	
	a->waitInitReading();
	b->waitInitReading();
	m_stats += MRInterMerger::merge(preload_tasks_launcher,
									a,
									b,
									result,
									m_MR,
									m_preload_buffer_size);
	a->deleteFile();
	b->deleteFile();
	result->waitFlushFinished();
	onGotResult(result);
	
	CallProgressBar();
	return 0;
}

void MRNodeDispatcher::onAddResult(MRInterResultPtr inter_result)
{
	inter_result->waitFlushFinished();
	CallProgressBar();
	//std::cout << "MRNodeDispatcher::onAddResultk inter_results.lock() ";
	inter_results.lock();
	nbatches++;
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
	
	//std::cout << "NBATCHES" << nbatches.load() << std::endl;
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
	
	if (nbatches.load()==1 && nomore_inter)
	{
		onReducesFinished();
	}
	
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
