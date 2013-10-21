#include "InterResultReader.h"

#include <sys/types.h>
#include <sys/mman.h>

#include <dirent.h>

#include <sys/uio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/uio.h> 
#include <unistd.h>

InterResultLoader::InterResultLoader(std::string filename, EmitDumperPtr dumper):
	m_dumper(dumper)
{
	m_fd = open(filename.c_str(),  O_RDONLY,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	m_len = lseek(m_fd, 0, SEEK_END);
	lseek(m_fd, 0, SEEK_SET);
	p = (uint8_t *)mmap(0, m_len, PROT_READ, MAP_SHARED, m_fd, 0);
}

InterResultLoader::~InterResultLoader()
{
	munmap(p, m_len);
	close(m_fd);
}

EmitType *InterResultLoader::readEmit(off_t offset)
{
	uint8_t *data = p+offset;
	uint64_t key = *((uint64_t*)data);
	uint64_t size = *((uint64_t*)(data+sizeof(uint64_t)));
	char bf[size+1];
	memcpy(bf, (data+2*sizeof(uint64_t)), size);
	bf[size] = '\0';
	
	return m_dumper->restore(std::string(bf));
}
