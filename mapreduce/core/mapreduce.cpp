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
