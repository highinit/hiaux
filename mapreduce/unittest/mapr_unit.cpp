#include "mapr_unit.h"
#include "../core/MRNodeDispatcher.h"

#include <sys/types.h>
#include <dirent.h>

#include <sys/uio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/uio.h> 
#include <unistd.h>

#include <boost/bind.hpp>

#include "../../threadpool/threadpool.h"

void MaprTests::testInvLineDumper()
{
	std::cout << "MaprTests::testInvLineDumper()\n";
	InvertLineDumper* dumper = new InvertLineDumper;
	int fd = open("dumptest",  O_RDWR | O_CREAT,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	InvertLine* line = new InvertLine(0);
	line->pages.push_back(0);

	InvertLine* line2 = (InvertLine*)dumper->restore( dumper->dump(line) );
	if (line2->key != line->key)
	{
		std::cout << "MaprTests::testInvLineDumper keys different\n"; 
		exit(0);
	}

	if (line2->pages[0] != line->pages[0])
	{
		std::cout << "MaprTests::testInvLineDumper pages different\n"; 
		exit(0);
	}
}

void MaprTests::testMRInterResult()
{
	int fd = open("inter",  O_RDWR | O_CREAT | O_TRUNC,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	MRInterResult *inter = new MRInterResult(fd, new InvertLineDumper);

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

	std::vector<int64_t> keys = inter->getKeys();

	for (int i = 0; i<keys.size(); i++)
	{
		inter->preload(keys[i], cid);
		InvertLine *line = (InvertLine*)
					inter->getEmit(keys[i], cid);
		if (line->key != keys[i])
		{
			std::cout << "TEST FAILED: different keys\n";
			exit(0);
		}
		std::cout << line->pages.size() << " " <<  std::endl;

		/*if (line->pages[0] != keys[i])
		{
			std::cout << "TEST FAILED: different emits\n";
			exit(0);
		}*/
	}


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

void MaprTests::loadCache(MRInterResult *inter,
				bool cid,
				std::vector<int64_t> &keys, int b, int e)
{
	load_lock.lock();
	//std::cout << "loaded " << b << " to " << e << std::endl;
	for (int i = b; i<e; i++)
	{
		inter->preload(keys[i], cid);
	}
	inter->setCacheReady(cid);
	//std::cout << "loaded Cache\n";
	load_lock.unlock();
}

void MaprTests::testMRInterResultAsync()
{
	hThreadPool *pool = new hThreadPool(1);
	pool->run();
	
	int fd = open("testMRInterResultAsync",  O_RDWR | O_CREAT | O_TRUNC,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	MRInterResult *inter = new MRInterResult(fd, new InvertLineDumper);
	
	const int nemits = 100;
	const int nparts = 10;
	int emits_per_part = nemits/nparts;
	
	for (int64_t i = 1; i<=nemits; i++)
	{
		InvertLine* line = new InvertLine(i);
		for (int j = i; j<i+1000; j++)
		line->pages.push_back(j);
		//std::cout << line->pages[0] << " " <<  std::endl;
		inter->addEmit(i, line);
	}
	
	std::cout << "file written\n";
	std::vector<int64_t> keys = inter->getKeys();
	
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
			inter->getEmit(keys[i], cid);
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
	
	std::cout << "FINISHED\n";;
	
	exit(0);
}

int main(int argc, char **argv)
{
	std::cout << "MaprTests::testMRInterResult\n";
	MaprTests tests;
	//tests.testInvLineDumper();
	//tests.testMRInterResult();
	tests.testMRInterResultAsync();
	
	std::cout << "all tests ended\n";
	return 0;
}
