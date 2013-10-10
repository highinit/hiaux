#include "FileCartulary.h"

#include <sys/types.h>
#include <dirent.h>

#include <sys/uio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/uio.h> 
#include <unistd.h>

int ReadFileRent::getReadFile(int fileid)
{
	std::unordered_map<int, int>::iterator it = filecache.find(fileid);
	if (it != filecache.end())
	{
		return it->second;
	}
	else
	{
		int fd = open(m_filename_from_id(fileid).c_str(), O_RDONLY,  
					   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		filecache.insert(std::pair<int, int>(fileid, fd));
		return fd;
	}
}

ReadFileRent::ReadFileRent(boost::function<std::string(int id)> filename_from_id):
	m_filename_from_id (filename_from_id)
{
	
}

ReadFileRent::~ReadFileRent()
{
	std::unordered_map<int, int>::iterator it = filecache.begin();
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
		int fd = open(m_filename_from_id(id).c_str(), O_WRONLY | O_CREAT | O_APPEND,   
					   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		filecache.insert(std::pair<int, int>(id, fd));
		return fd;
	}
}

void AppendFileDeposit::close()
{
	std::unordered_map<int, int>::iterator it = filecache.begin();
	while (it != filecache.end())
	{
		::close(it->second);
		filecache.erase(it);
		it++;
	}
}

AppendFileDeposit::~AppendFileDeposit()
{
	close();
}