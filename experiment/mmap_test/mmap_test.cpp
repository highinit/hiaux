
#include <sys/types.h>
#include <sys/mman.h>

#include <dirent.h>

#include <sys/uio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/uio.h> 
#include <unistd.h>

#include <iostream>

void writeShit()
{
	int fd = open("testmapfile",  O_RDWR | O_CREAT | O_TRUNC,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	
	for (int i = 0; i<3; i++)
	{
		int64_t key = 42+i;
		char bf[50];
		sprintf(bf, "_value_%d_", i);
		std::string dump(bf);
		int64_t size = dump.size();

		iovec atom[3];
		atom[0].iov_base =  &key;
		atom[0].iov_len = sizeof(int64_t);

		atom[1].iov_base =  &size;
		atom[1].iov_len = sizeof(int64_t);

		atom[2].iov_base = (void*)dump.data();
		atom[2].iov_len = size;

		writev(fd, atom, 3);

		
	}
	close(fd);
}

void checkShit()
{
	int fd = open("testmapfile",  O_RDONLY,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	
	size_t len = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	
	uint8_t *p = (uint8_t *)mmap(0, len, PROT_READ, MAP_SHARED, fd, 0);
	size_t nread = 0;
	
	while (nread < len)
	{
		int64_t key = *((int64_t*)p);
		int64_t size = *((int64_t*)(p+8));
		char bf[size+1];
		memcpy(bf, (p+8+8), size);
		bf[size] = '\0';

		std::cout << "key: " << key << std::endl;
		std::cout << "value: " << bf << std::endl;
		
		nread += 8+8+size;
		p = p + 8+8+size;
	}
	
	munmap(p, len);
}


int main()
{
	std::cout << "mmap test" << std::endl;
	
	writeShit();
	checkShit();
	
	return 0;
}
