#include "hiconfig.h"
#include "pq.h"

PQ::PQ(const std::string &_host,
		const std::string &_db,
		const std::string &_user,
		const std::string &_pass):
	m_host(_host),
	m_db(_db),
	m_user(_user),
	m_pass(_pass)
{
}

PQ::~PQ()
{	
}
