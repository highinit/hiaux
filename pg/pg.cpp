#include "hiconfig.h"
#include "pg.h"

PG::PG(const std::string &_host,
		const std::string &_db,
		const std::string &_user,
		const std::string &_pass):
	m_host(_host),
	m_db(_db),
	m_user(_user),
	m_pass(_pass)
{
	std::string params = "hostaddr = '" + _host + "' port = '5432' dbname = '"
			 			+ _db + "' user = '" + _user + "' password = '" 
						+ _pass + "' connect_timeout = '10'";   
	
	m_conn = PQconnectdb(params.c_str());
	
	if (!m_conn) {
		std::cerr << "PG::PG !m_conn" << std::endl;
	}
	
	if (PQstatus(m_conn) != CONNECTION_OK) {
		std::cerr << "PG::PG PQstatus(m_conn) != CONNECTION_OK" << std::endl;
	}
}

PGresult* PG::query(const std::string &_q) {
	return PQexec(m_conn, _q.c_str());
}

PG::~PG()
{
}
