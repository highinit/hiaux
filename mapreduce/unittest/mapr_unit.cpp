#include "mapr_unit.h"
#include "../core/MRNodeDispatcher.h"

#include "../core/MRInterMerger.h"

#include <sys/types.h>
#include <dirent.h>

#include <sys/uio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/uio.h> 
#include <unistd.h>

#include <boost/bind.hpp>

#include <iomanip>

#include "../../threadpool/threadpool.h"

void MaprTests::testInvLineDumper()
{
	/*
	std::cout << "MaprTests::testInvLineDumper()\n";
		int fd = open("dumptest",  O_RDWR | O_CREAT,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	InvertLine* line = new InvertLine();
	line->pages.push_back(0);

	InvertLine* line2 = (InvertLine*)dumper->restore( dumper->dump(line) );
	 */
	 
	/*if (line2->key != line->key)
	{
		std::cout << "MaprTests::testInvLineDumper keys different\n"; 
		exit(0);
	}*/

	/*
	if (line2->pages[0] != line->pages[0])
	{
		std::cout << "MaprTests::testInvLineDumper pages different\n"; 
		exit(0);
	}
	 * */
}

void MaprTests::testMRInterResult()
{/*
	MRInterResult *inter = new MRInterResult("inter", new InvertLineDumper);

	const int64_t nemits = 10;
//	const int nparts = 10;
//	int nemits_per_part = nemits/nparts;

	bool cid = 0;
	for (int64_t i = 0; i<=nemits; i++)
	{
		InvertLine* line = new InvertLine(i);
		line->pages.push_back(i);
		//std::cout << line->pages[0] << " " <<  std::endl;
		inter->addEmit(i, line);
	}

	Int64VecPtr keys = inter->getKeys();

	for (int i = 0; i<keys->size(); i++)
	{
		inter->preload(keys->at(i), cid);
		InvertLine *line = (InvertLine*)
					inter->getEmit(keys->at(i), cid);
		if (line->key != keys->at(i))
		{
			std::cout << "TEST FAILED: different keys\n";
			exit(0);
		}
		std::cout << line->pages.size() << " " <<  std::endl;

	}
*/

	int part_begin_key_i = 0;


/*	for (int i = 0; i<nparts; i++)
	{
		for (int j = 0; j<nemits_per_part; j++)
		{
			inter.preload(keys[part_begin_key_i+j], cid);
		}
		
		for (int j = 0; j<nemits_per_part; j++)
		{
			InvertLine *line = (InvertLine*)
						inter.getEmit(keys[part_begin_key_i+j], cid);
			if (line->key != keys[part_begin_key_i+j])
			{
				std::cout << "TEST FAILED: different keys\n";
				exit(0);
			}
	//		std::cout << line->pages[i] << std::endl;
			
			if (line->pages[0] != keys[part_begin_key_i+j])
			{
				std::cout << "TEST FAILED: different emits\n";
				exit(0);
			}
		}
		
		part_begin_key_i += nemits_per_part;
		inter.clearCache(cid);
	}
	*/
}

bool MaprTests::loadCache(MRInterResult *inter,
				bool cid,
				Int64VecPtr keys, int b, int e)
{
	load_lock.lock();
	//std::cout << "loaded " << b << " to " << e << std::endl;
	for (int i = b; i<e; i++)
	{
		inter->preload(keys->at(i), cid);
	}
	inter->setCacheReady(cid);
	//std::cout << "loaded Cache\n";
	load_lock.unlock();
	return 0;
}

void MaprTests::testMRInterResultAsync()
{
	hThreadPool *pool = new hThreadPool(1);
	pool->run();
	TaskLauncher flush_tasks_launcher(pool, 1, boost::bind(&MaprTests::onMRInterMergerFinished, this));
	
	MRInterResult *inter = new MRInterResult("testMRInterResultAsync",
						new MapReduceInvertIndex,
						flush_tasks_launcher);
	
	const int nemits = 100;
	const int nparts = 10;
	int emits_per_part = nemits/nparts;
	
	for (int64_t i = 1; i<=nemits; i++)
	{
		InvertLine* line = new InvertLine();
		for (int j = i; j<i+1000; j++)
		line->pages.push_back(j);
		//std::cout << line->pages[0] << " " <<  std::endl;
		inter->addEmit(i, line);
	}
	
	std::cout << "file written\n";
	Int64VecPtr keys = inter->getKeys();
	
	int key_r_b = 0;
	int key_r_e = emits_per_part;
	
	bool cid = 0;
	int key_b = 0;
	int key_e = fmin(key_b+emits_per_part, nemits);
	
	loadCache(inter, 0, keys, key_b, key_e);	
	key_b += emits_per_part;
	key_e = fmin(key_b+emits_per_part, nemits);
	loadCache(inter, 1, keys, key_b, key_e);	
	
	std::cout << "init caches loaded\n";
	int nread = 0;
	while (1)
	{	
		//std::cout << "waiting cache\n";
		inter->condWaitCache(cid);
		
		//std::cout << "reading from cache " << key_r_b << " to " << key_r_e << "\n";
		nread += key_r_e - key_r_b;
		// get
		
		for (int i = key_r_b; i<key_r_e; i++)
		{
			inter->getEmit(keys->at(i), cid);
		}
		
		key_r_b += emits_per_part;
		key_r_e = fmin(key_r_b+emits_per_part, nemits);
	
		if (key_r_b >= nemits)
		{
			std::cout << "read all: " << nread << std::endl;
			exit(0);
			//break;
		}
	
		key_b += emits_per_part;
		key_e = fmin(key_b+emits_per_part, nemits);
		
		/*if (key_b >= nemits)
		{
			continue;
		}*/
		
		inter->clearCache(cid);
		pool->addTask(new boost::function<void()>( boost::bind(&MaprTests::loadCache,
								this,
								inter,
								cid,
								keys,
								key_b,
								key_e)));
		cid = !cid;
	}
	
	std::cout << "FINISHED\n";
	
	exit(0);
}

void MaprTests::onMRInterMergerFinished()
{
	std::cout << "finished\n";
}

void MaprTests::testMRInterMerger()
{
	std::cout << "MaprTests::testMRInterMerger\n";
	
	hThreadPool *pool = new hThreadPool(8);
	pool->run();

	TaskLauncher flush_tasks_launcher(pool, 1, boost::bind(&MaprTests::onMRInterMergerFinished, this));

	MapReduceInvertIndex *MR = new MapReduceInvertIndex();
	
	MRInterResultPtr inter1 (new MRInterResult(m_path+"inter1", MR, flush_tasks_launcher));
	MRInterResultPtr inter2 (new MRInterResult(m_path+"inter2", MR, flush_tasks_launcher));
	MRInterResultPtr inter3 (new MRInterResult(m_path+"inter3", MR, flush_tasks_launcher));
	
	int nkeys = 10000000;
	int keys_in_cache = 100000;
	
	for (uint64_t i = 1; i<=2*nkeys/3; i++)
	{
		InvertLine* line = new InvertLine();
		for (int j = i; j<i+50; j++)
			line->pages.push_back(j);
		inter1->addEmit(i, line);
	}
	
	for (uint64_t i = nkeys/3; i<=nkeys; i++)
	{
		InvertLine* line = new InvertLine();
		for (int j = i; j<i+50; j++)
			line->pages.push_back(j);
		inter2->addEmit(i, line);
	}
	std::cout << "emits created\n";
	int64_t ts_start = time(0);
	
	inter1->waitFlushFinished();
	inter2->waitFlushFinished();
	
	inter1->waitInitReading();
	inter2->waitInitReading();
	
	//inter1->setModeReading();
	//inter2->setModeReading();
	
	TaskLauncher preload_tasks_launcher(pool, 1, boost::bind(&MaprTests::onMRInterMergerFinished, this));
	
	MRInterMerger::merge(preload_tasks_launcher, inter1, inter2, inter3, MR, keys_in_cache);
	
	inter3->waitFlushFinished();
	//std::cout << "result keys:" << inter3->getKeys()->size() << std::endl;
	std::cout << "time took " << time(0) - ts_start << std::endl; 
	// 40 sec, 500mb
	
	//Int64VecPtr keys = inter3->getKeys();
	//std::cout << "nkeys: " << keys->size() << std::endl; 
	
	/*for (int i = 0; i<keys->size(); i++)
	{
		inter3->preload(keys->at(i), 0);
		InvertLine* line = (InvertLine*)inter3->getEmit(keys->at(i), 0);
		
		std::cout << "key: " << keys->at(i) << " ";
		line->print();
	}*/
}

void MaprTests::onGotResult(MRInterResultPtr res)
{
	res->waitFlushFinished();
	std::cout << "got result\n";
}

void MaprTests::onBatchingFinished()
{
	std::cout << "Batching finished\n";
	exit(0);
}

void MaprTests::testBatcher()
{
	std::cout << "MaprTests::testBatcher\n";
	MapReduceInvertIndex *MR = new MapReduceInvertIndex();
	hThreadPool *pool = new hThreadPool(10);
	pool->run();
	TaskLauncher *flush_launcher = 
			new TaskLauncher(pool, 1, boost::bind(&MaprTests::onMRInterMergerFinished, this));
	
	mr_disp = new MRBatchDispatcher(MR, 
									pool,
									1,
									*flush_launcher,
									m_path,
									boost::bind(&MaprTests::onGotResult, this, _1),
									boost::bind(&MaprTests::onBatchingFinished, this));

	std::vector<Document*> docs;
	
	// keys: 4000000
	const int input_size = 50;
	int nemits = 4000; //4000000
	
	for (int i = 0; i<=input_size; i++)
	{
		Document *doc = new Document((nemits/50)*i, (nemits/50)*i+(nemits/50), i);
		docs.push_back( doc );
		if (i%(input_size/10) ==0)
		{
			DocumentBatch *batch = new DocumentBatch(docs);
			docs.clear();
			//batches->push_back(batch);
			mr_disp->addBatch(batch);
		}
	}
	mr_disp->noMore();

	pool->join();
}

void testNodeDispatcherFinished(MRInterResultPtr result)
{
//	return;
	std::cout << "testNodeDispatcherFinished\n";
	//exit(0);
	int fd = open("merge8", O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	
	uint64_t key;
	uint64_t size;
	
	MapReduceInvertIndex *MR = new MapReduceInvertIndex();
	
	while (1)
	{
		if (read(fd, &key, sizeof(uint64_t)) != sizeof(uint64_t))
		{
			exit(0);
		}
		read(fd, &size, sizeof(uint64_t));
		char bf[size+1];
		read(fd, bf, size);
		bf[size] = '\0';
		
		InvertLine *line = (InvertLine*)MR->restoreEmit(std::string(bf));
		if (line->pages[0] != key)
		std::cout << line->pages[0] << " ";
	}
	
}

void showProgress(MRProgressBar bar)
{
	
	std::cout << "PROGRESS MAP: " << bar.map_p << " REDUCE: "
			<< std::setiosflags(std::ios::fixed) <<
			std::setprecision(1) << bar.red_p << std::endl;
}

void MaprTests::testNodeDispatcher()
{
	std::cout << "MaprTests::testNodeDispatcher\n";
	hThreadPool *pool = new hThreadPool(8);
	pool->run();
	MRNodeDispatcher *node = new MRNodeDispatcher(pool, 
											new MapReduceInvertIndex,
											m_path,
											boost::bind(&testNodeDispatcherFinished, _1),
											6,
											6);
	
	node->setProgressBar(boost::bind(&showProgress, _1));
	
	std::vector<Document*> docs;
	
	// keys: 4000000
	const int input_size = 1000000;
//	int nemits = 4000000; //4000000
	
	/*
	for (int i = 1; i<=input_size; i++)
	{
		Document *doc = new Document(i, i+1000, i);
		docs.push_back( doc );
		if (i%(input_size/10) ==0)
		{
			DocumentBatch *batch = new DocumentBatch(docs);
			docs.clear();
			//batches->push_back(batch);
			node->addBatch(batch);
		}
	}
	 */
	for (int i = 0; i<=input_size; i++)
	{
		Document *doc = new Document(i, i+1, i);
		docs.push_back( doc );
		if (i%(input_size/1000) ==0)
		{
			DocumentBatch *batch = new DocumentBatch(docs);
			docs.clear();
			//batches->push_back(batch);
			node->addBatch(batch);
		}
	}
	
	node->noMoreBatches();
	pool->join();
}

int main(int argc, char **argv)
{
	try
	{
		MaprTests tests;
		tests.m_path = "";
		//tests.testInvLineDumper();
		//tests.testMRInterResult();
		//tests.testMRInterResultAsync();
		//tests.testMRInterMerger();
		//tests.testBatcher();
		tests.testNodeDispatcher();

		std::cout << "all tests ended\n";
	}
	catch (const char *a)
	{
		std::cout << "EX: " << a << std::endl;
	} 
	
	return 0;
}
