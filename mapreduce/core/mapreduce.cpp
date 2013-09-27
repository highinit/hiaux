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

MapReduce::MapReduce (std::string job_name, std::string node_name)
{
    m_job_name = job_name;
    m_node_name = node_name;
}

void MapReduce::setEmitF(boost::function<void(int64_t, EmitType*)> emitf)
{
    emit = emitf;
}