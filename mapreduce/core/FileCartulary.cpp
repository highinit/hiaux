#include "FileCartulary.h"

#include <sys/types.h>
#include <dirent.h>

#include <sys/uio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/uio.h> 
#include <unistd.h>

int ReadFileRent::getReadFile(std::string filename)
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

ReadFileRent::~ReadFileRent()
{
	std::unordered_map<std::string, int>::iterator it = filecache.begin();
	while (it != filecache.end())
	{
		close(it->second);
		filecache.erase(it);
		it++;
	}
}

AppendFileDeposit::AppendFileDeposit(boost::function<std::string(int id)> \
				filename_from_id):
	m_filename_from_id (filename_from_id)
{
}

int AppendFileDeposit::getAppendFile(int id)
{
	std::unordered_map<int, int>::iterator it = filecache.find(id);
	if (it != filecache.end())
	{
		return it->second;
	}
	else
	{
		int fd = open(m_filename_from_id(id).c_str(), O_APPEND, O_CREAT,   
					   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		filecache.insert(std::pair<int, int>(id, fd));
		return fd;
	}
}

AppendFileDeposit::~AppendFileDeposit()
{
	std::unordered_map<int, int>::iterator it = filecache.begin();
	while (it != filecache.end())
	{
		close(it->second);
		filecache.erase(it);
		it++;
	}
}