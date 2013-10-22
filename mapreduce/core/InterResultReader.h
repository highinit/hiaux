#ifndef INTER_RESULT_LOADER_H
#define INTER_RESULT_LOADER_H

#include "mapreduce.h"

class InterResultLoader
{
	int m_fd; 
	size_t m_len;
	uint8_t *p;
	MapReduce *m_MR;
	
	InterResultLoader();
	InterResultLoader(InterResultLoader&);
	InterResultLoader& operator=(InterResultLoader&);
public:
	InterResultLoader(std::string filename, MapReduce *MR);
	~InterResultLoader();
	
	EmitType *readEmit(off_t offset);
};

#endif