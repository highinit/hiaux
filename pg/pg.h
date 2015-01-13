#ifndef _HIAUX_PQ_H_
#define _HIAUX_PQ_H_

#include "hiconfig.h"

#include "hiaux/strings/string_utils.h"

#include <libpq-fe.h>
//#include "hiaux/threads/locks.h"
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <string>

class PGNoConnEx {
};

class PGCantQueryEx {
};

class PG : public boost::noncopyable {
	const std::string m_host;
	const int m_port;
	const std::string m_db;
	const std::string m_user;
	const std::string m_pass;
	
	
	
	bool doCheckDbConn(size_t _attempt);
	bool checkDbConn();
	
	PGresult* doQuery(const std::string &_q, int attempt);
	
public:
	
	PGconn *m_conn;
	
	PG(const std::string &_host,
		int _port, 
		const std::string &_db,
		const std::string &_user,
		const std::string &_pass);
	
	PGresult* query(const std::string &_q);
	
	virtual ~PG();
};

typedef boost::shared_ptr<PG> PGPtr;

#endif
