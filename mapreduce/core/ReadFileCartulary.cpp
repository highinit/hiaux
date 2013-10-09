#include "ReadFileCartulary.h"

#include <sys/types.h>
#include <dirent.h>

#include <sys/uio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/uio.h> 
#include <unistd.h>

int ReadFileCartulary::getReadFile(std::string filename)
{
	std::unordered_map<std::string, int>::iterator it = filecache.find(filename);
	if (it != filecache.end())
	{
		return it->second;
	}
	else
	{
		int fd = open(filename.c_str(), O_RDONLY,  
					   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		filecache.insert(std::pair<std::string, int>(filename, fd));
		return fd;
	}
}

ReadFileCartulary::~ReadFileCartulary()
{
	std::unordered_map<std::string, int>::iterator it = filecache.begin();
	while (it != filecache.end())
	{
		close(it->second);
		filecache.erase(it);
		it++;
	}
}
