/*
 * Copyright (c) 2010-2013  Artur Gilmutdinov

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the BSD 2-Clause License

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * BSD 2-Clause License for more details.

 * You should have received a copy of the BSD 2-Clause License
 * along with this program; if not, see <http://opensource.org/licenses>.

 * See also <http://highinit.com>
*/

#include "mapreduce.h"

#include <sys/types.h>
#include <dirent.h>

#include <sys/uio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/uio.h> 
#include <unistd.h>

MRStats::MRStats()
{
	nmaps = 0;
	nemits = 0;
	nreduces = 0;
}

MRStats::MRStats(MRStats &a)
{
	nmaps = a.nmaps.load();
	nemits = a.nemits.load();
	nreduces = a.nreduces.load();
}

MRStats& MRStats::operator+=(const MRStats &a)
{
	nmaps = nmaps.load() + a.nmaps.load();
	nemits = nemits.load() + a.nemits.load();
	nreduces = nreduces.load() + a.nreduces.load();
	return *this;
}

MRStats& MRStats::operator=(const MRStats &a)
{
	nmaps = a.nmaps.load();
	nemits = a.nemits.load();
	nreduces = a.nreduces.load();
	return *this;
}

MapReduce::MapReduce()
{

}

void MapReduce::setEmitF(boost::function<void(uint64_t, EmitType*)> emitf)
{
    emit = emitf;
}

EmitTypeAccessor::EmitTypeAccessor(EmitType *emit, EmitDumper *dumper, int append_fd, int emitter_id):
	m_emit(emit),
	m_emitter_id(emitter_id)
{
	// dump and remember offset and size
	std::string	dump = dumper->dump(emit);
	delete m_emit;
	//std::cout << dump <<std::endl;
	m_offset = lseek(append_fd, 0, SEEK_CUR);
	size_t size = dump.size();
	
	iovec atom[2];
	atom[0].iov_base =  &size;
	atom[0].iov_len = sizeof(size_t);

	atom[1].iov_base = (void*)dump.data();
	atom[1].iov_len = size;

	writev(append_fd, atom, 2);
	dump.clear();
}

int EmitTypeAccessor::getEmitterId()
{
	return m_emitter_id;
}

void EmitTypeAccessor::restore(EmitDumper *dumper, int read_fd)
{
	lseek(read_fd, m_offset, SEEK_SET);
	size_t size;
	if (read(read_fd, &size, sizeof(size_t))!=sizeof(size_t))
	{
		std::cout << "READ ERROR\n";
		exit(0);
	}
	char *bf = new char [size+1];
	if (read(read_fd, bf, size)!=size)
	{
		std::cout << "READ ERROR\n";
		exit(0);
	}
	bf[size] = '\0';
	
	//m_emit->restore(dump);
	m_emit = dumper->restore(std::string(bf));
	delete [] bf;
}

EmitType *EmitTypeAccessor::getEmit()
{
	return m_emit;
}
