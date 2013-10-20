#ifndef INTER_RESULT_LOADER_H
#define INTER_RESULT_LOADER_H

#include "mapreduce.h"

class InterResultLoader
{
	int m_fd; 
	size_t m_len;
	uint8_t *p;
	EmitDumper *m_dumper;
	
	InterResultLoader();
	InterResultLoader(InterResultLoader&);
	InterResultLoader& operator=(InterResultLoader&);
public:
	InterResultLoader(std::string filename, EmitDumper *dumper);
	~InterResultLoader();
	
	EmitType *readEmit(size_t offset);
};

#endif