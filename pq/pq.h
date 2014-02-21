#ifndef _HIAUX_PQ_H_
#define _HIAUX_PQ_H_

#include "hiconfig.h"

#include <libpq-fe.h>
//#include "hiaux/threads/locks.h"
#include <boost/noncopyable.hpp>

#include <string>

class PQ : public boost::noncopyable {
	const std::string m_host;
	const std::string m_db;
	const std::string m_user;
	const std::string m_pass;
	PGconn *m_conn;
public:
	
	PQ(const std::string &_host,
		const std::string &_db,
		const std::string &_user,
		const std::string &_pass);
	~PQ();
};

#endif
