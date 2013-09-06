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