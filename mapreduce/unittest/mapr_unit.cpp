#include "mapr_unit.h"

#include <sys/types.h>
#include <dirent.h>

#include <sys/uio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/uio.h> 
#include <unistd.h>

void MaprTests::testInvLineDumper()
{
	std::cout << "MaprTests::testInvLineDumper()\n";
	InvertLineDumper* dumper = new InvertLineDumper;
	int fd = open("dumptest",  O_RDWR | O_CREAT,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	
	InvertLine* line = new InvertLine(42);
	line->pages.push_back(4242);
	
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
	int fd = open("inter",  O_RDWR | O_CREAT,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	MRInterResult inter(fd, new InvertLineDumper);
	
	const int nemits = 10;
//	const int nparts = 10;
//	int nemits_per_part = nemits/nparts;

	bool cid = 0;
	for (int i = 0; i<nemits; i++)
	{
		InvertLine* line = new InvertLine(i);
		line->pages.push_back(i);
		//std::cout << line->pages[0] << " " <<  std::endl;
		inter.addEmit(i, line);
	}
	
	std::vector<int64_t> keys = inter.getKeys();
	
	for (int i = 0; i<keys.size(); i++)
	{
		inter.preload(keys[i], cid);
		InvertLine *line = (InvertLine*)
					inter.getEmit(keys[i], cid);
		if (line->key != keys[i])
		{
			std::cout << "TEST FAILED: different keys\n";
			exit(0);
		}
		//std::cout << line->pages[0] << " " <<  std::endl;

		if (line->pages[0] != keys[i])
		{
			std::cout << "TEST FAILED: different emits\n";
			exit(0);
		}
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

int main(int argc, char **argv)
{
	std::cout << "MaprTests::testMRInterResult\n";
	MaprTests tests;
	//tests.testInvLineDumper();
	tests.testMRInterResult();
	
	std::cout << "all tests ended\n";
	return 0;
}
