#ifndef _HIAUX_PQ_H_
#define _HIAUX_PQ_H_

#include "hiconfig.h"

#include <libpq-fe.h>
//#include "hiaux/threads/locks.h"
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <string>

class PG : public boost::noncopyable {
	const std::string m_host;
	const std::string m_db;
	const std::string m_user;
	const std::string m_pass;
	PGconn *m_conn;
	
	bool doCheckDbConn(size_t _attempt);
	bool checkDbConn();
	
public:
	
	PG(const std::string &_host,
		const std::string &_db,
		const std::string &_user,
		const std::string &_pass);
	
	PGresult* query(const std::string &_q);
	
	virtual ~PG();
};

typedef boost::shared_ptr<PG> PGPtr;

#endif
