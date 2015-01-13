#include "hiconfig.h"
#include "pg.h"

PG::PG(const std::string &_host,
		int _port,
		const std::string &_db,
		const std::string &_user,
		const std::string &_pass):
	m_host(_host),
	m_port(_port),
	m_db(_db),
	m_user(_user),
	m_pass(_pass)
{
	std::string params = "hostaddr = '" + _host + "' port = '" + inttostr(_port) + "' dbname = '"
			 			+ _db + "' user = '" + _user + "' password = '" 
						+ _pass + "' connect_timeout = '10'";   
	
	m_conn = PQconnectdb(params.c_str());
	
	if (!m_conn) {
		std::cerr << "PG::PG !m_conn" << std::endl;
		throw PGNoConnEx();
	}
	
	if (PQstatus(m_conn) != CONNECTION_OK) {
		std::cerr << "PG::PG PQstatus(m_conn) != CONNECTION_OK" << std::endl;
		throw PGNoConnEx();
	}
}

PGresult* PG::doQuery(const std::string &_q, int _attempt) {
	
	if (_attempt > 10)
		throw PGCantQueryEx();
	
	PGresult *res = PQexec(m_conn, _q.c_str());
	
	ExecStatusType status = PQresultStatus(res);
	
	if (status != PGRES_TUPLES_OK &&
		status != PGRES_COMMAND_OK &&
		status != PGRES_EMPTY_QUERY &&
		status != PGRES_NONFATAL_ERROR) {
		
		if (checkDbConn()) {
			
			return doQuery(_q, _attempt + 1);
			
		} else {
			
			throw PGCantQueryEx();
		}
	}
	
	return res;
}

PGresult* PG::query(const std::string &_q) {
	
	return doQuery(_q, 0);
}

bool PG::doCheckDbConn(size_t _attempt) {
	
	if (_attempt > 10)
		return false;
	
	if (PQstatus(m_conn) != CONNECTION_OK) {
		
		std::cout << "Reseting connection to PostgreSQL\n";
		PQreset(m_conn);
		return doCheckDbConn(_attempt + 1);
	}
	return true;
}

bool PG::checkDbConn() {
	
	return doCheckDbConn(0);
}

PG::~PG() {
	
	PQfinish(m_conn);
}
