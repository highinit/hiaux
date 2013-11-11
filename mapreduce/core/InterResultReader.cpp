#include "InterResultReader.h"

#include <sys/types.h>
#include <sys/mman.h>

#include <dirent.h>

#include <sys/uio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/uio.h> 
#include <unistd.h>

#include "../../common/hexception.h"

InterResultLoader::InterResultLoader(std::string filename, MapReduce *MR):
	m_MR(MR),
	closed(0),
	m_filename(filename)
{
	m_fd = open(filename.c_str(),  O_RDONLY,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (m_fd == -1)
	{
		throw hException("InterResultLoader:: open "+std::string(strerror(errno)));
	}
	
	m_len = lseek(m_fd, 0, SEEK_END);
	if (m_len == -1)
	{
		throw hException("InterResultLoader:: lseek "+std::string(strerror(errno)));
	}
	
	if (lseek(m_fd, 0, SEEK_SET) == -1)
	{
		throw hException("InterResultLoader:: lseek "+std::string(strerror(errno)));
	}
	
	p = (uint8_t *)mmap(0, m_len, PROT_READ, MAP_SHARED, m_fd, 0);
	if ((void*)p == (void*)-1)
	{
		throw hException("InterResultLoader:: mmap "+std::string(strerror(errno)));
	}
}

InterResultLoader::~InterResultLoader()
{
	if (!closed)
	{
		if (munmap(p, m_len) == -1)
		{
			throw hException("InterResultLoader::~InterResultLoader:: munmap "
					+std::string(strerror(errno)));
		}
		if (close(m_fd) == -1)
		{
			throw hException("InterResultLoader::~InterResultLoader:: close "
					+std::string(strerror(errno)));
		}
	}
}

void InterResultLoader::deleteFile()
{
	if (munmap(p, m_len) == -1)
	{
		throw hException("InterResultLoader::deleteFile:: munmap "+std::string(strerror(errno)));
	}
	
	if (close(m_fd) == -1)
	{
		throw hException("InterResultLoader::deleteFile:: close "
				+std::string(strerror(errno)));
	}
	
	if (unlink(m_filename.c_str()) == -1)
	{
		throw hException("InterResultLoader::deleteFile:: unlink "+std::string(strerror(errno)));
	}
	
	closed = 1;
}

FileMapPtr InterResultLoader::getFileMap()
{
	FileMapPtr filemap(new FileMap);
	uint8_t *data = p;
	uint64_t off = 0;
	
	while (off < m_len)
	{
		uint64_t key = *((uint64_t*)data);
		
		filemap->insert(std::pair<uint64_t, uint64_t>(key, off));
		
		uint64_t size = *((uint64_t*)(data+sizeof(uint64_t)));
		off += 2*sizeof(uint64_t)+size;
		data += 2*sizeof(uint64_t)+size;
	}
	return filemap;
}

EmitType *InterResultLoader::readEmit(off_t offset)
{
	uint8_t *data = p+offset;
	uint64_t key = *((uint64_t*)data);
	uint64_t size = *((uint64_t*)(data+sizeof(uint64_t)));
	char bf[size+1];
	memcpy(bf, (data+2*sizeof(uint64_t)), size);
	bf[size] = '\0';
	
	return m_MR->restoreEmit(std::string(bf));
}
