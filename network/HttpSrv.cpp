#include <queue>

#include "HttpSrv.h"

extern int errno;

HttpSrv::ResponseInfo::ResponseInfo(const std::string &_content_type,
					const std::string &_server_name):
		content_type(_content_type),
		server_name(_server_name)
{
}

HttpSrv::Request::Request(const std::string &_url) {
	//std::cout << "Request: " << _url << std::endl; 
	//parseGET(_url, values_GET);
}

bool HttpSrv::Request::getField(const std::string &_key, std::string &_value) {
	
	hiaux::hashtable<std::string, std::string>::iterator it = values_GET.find(_key);
	
	if (it != values_GET.end()) {
		_value = it->second;
		return true;
	} else
		return false;
}

bool HttpSrv::Request::getCookie(const std::string &_name, std::string &_value) {
	
	hiaux::hashtable<std::string, std::string>::iterator it = cookies.find(_name);
	if (it == cookies.end())
		return false;
	_value = it->second;
	return true;
}

std::string HttpSrv::Request::toJson() {
	
	json_t *root = json_object();
	json_object_set_new(root, "url", json_string(url.c_str()));
	json_object_set_new(root, "path", json_string(path.c_str()));
	
	json_t *get_arr = json_array();
	
	hiaux::hashtable<std::string, std::string>::iterator it = values_GET.begin();
	hiaux::hashtable<std::string, std::string>::iterator end = values_GET.end();
	
	while (it != end) {
		
		json_t *getobj = json_object();
		json_object_set_new(getobj, "k", json_string(it->first.c_str()));
		json_object_set_new(getobj, "v", json_string(it->second.c_str()));
		json_array_append_new(get_arr, getobj);
		it++;
	}
	json_object_set_new(root, "getparams", get_arr);
	
	char *str = json_dumps(root, JSON_COMPACT);
	std::string dump(str);
	free(str);
	
	json_decref(root);
	return dump;
}

HttpSrv::HttpSrv(TaskLauncherPtr launcher,
			const HttpSrv::ResponseInfo &resp_info,
			boost::function<void(HttpSrv::ConnectionPtr,
								HttpSrv::RequestPtr)> request_hdl):
		m_launcher(launcher),
		m_resp_info(new ResponseInfo(resp_info)),
		m_request_hdl(request_hdl) {
	
	m_poolserver.reset(new hPoolServer(launcher, 
					boost::bind(&HttpSrv::onRead, this, _1),
					boost::bind(&HttpSrv::onWrite, this, _1),
					boost::bind(&HttpSrv::onError, this, _1)));
}

HttpSrv::~HttpSrv() {
	m_poolserver->stop();
}

HttpSrv::ConnectionPtr HttpSrv::getHttpConn(int socket)
{
	hLockTicketPtr ticket = m_connections_lock.lock();
	ConnectionPtr http_conn;
	hiaux::hashtable<int, ConnectionPtr>::iterator it = 
							connections.find(socket); 
	if (it==connections.end()) {
		http_conn.reset(new Connection(socket, m_resp_info, boost::bind(&HttpSrv::onRequest, this, _1, _2)));
		connections.insert(std::pair<int,ConnectionPtr>(socket, http_conn));
		return http_conn;
	} else
		return it->second;
}

HttpSrv::ConnectionPtr HttpSrv::getHttpConnConst(int socket) {
	
	hLockTicketPtr ticket = m_connections_lock.lock();
	hiaux::hashtable<int, ConnectionPtr>::iterator it = 
							connections.find(socket); 
	if (it==connections.end())
		return ConnectionPtr();
	return it->second;
}

void HttpSrv::closeHttpConn(int socket)
{
	std::cout << "HttpSrv::closeHttpConn\n";
	hLockTicketPtr ticket = m_connections_lock.lock();
	ConnectionPtr http_conn;
	hiaux::hashtable<int, ConnectionPtr>::iterator it = 
							connections.find(socket); 
	if (it!=connections.end())
		connections.erase(it);
}

void HttpSrv::onRead(hPoolServer::ConnectionPtr _pool_conn) {
	
	std::cout << "HttpSrv::onRead\n";
	ConnectionPtr http_conn = getHttpConn(_pool_conn->m_sock);
	checkConnClose(_pool_conn, http_conn);
	http_conn->performRecv();
}

void HttpSrv::onWrite(hPoolServer::ConnectionPtr _pool_conn) {
	
	ConnectionPtr http_conn = getHttpConn(_pool_conn->m_sock);
	checkConnClose(_pool_conn, http_conn);
}

void HttpSrv::onError(hPoolServer::ConnectionPtr _pool_conn) {
	
	ConnectionPtr http_conn = getHttpConn(_pool_conn->m_sock);
	checkConnClose(_pool_conn, http_conn);
	std::cout << "HttpSrv::onError\n";
	http_conn->close();
}

void HttpSrv::onRequest(int _fd, HttpSrv::RequestPtr _req) {
	
	std::cout << "HttpSrv::onRequest\n";
	m_request_hdl(getHttpConnConst(_fd), _req);
	ConnectionPtr http_conn = getHttpConn(_fd);
	
	hPoolServer::ConnectionPtr pool_conn = m_poolserver->getConnection(_fd);
	if (!pool_conn)
		closeHttpConn(_fd);
	else
		checkConnClose(pool_conn, http_conn);
}

void HttpSrv::checkConnClose(hPoolServer::ConnectionPtr _pool_conn, ConnectionPtr _conn) {
	
	
	uint64_t now = time(0);

	if (!_conn->alive || _conn->closing || now - _pool_conn->getCreateTs()>5) {
		
		closeHttpConn(_pool_conn->m_sock);
		_pool_conn->close();
		return;
	}
}

/*
void HttpSrv::handler(hPoolServer::ConnectionPtr pool_conn)
{
	//std::cout << "n http conn: " << connections.size() << std::endl;
	ConnectionPtr http_conn = getHttpConn(pool_conn->m_sock);
	
	RequestPtr req = http_conn->getNextRequest();

	uint64_t now = time(0);

	if (!http_conn->alive || http_conn->closing || now-pool_conn->getCreateTs()>15) {
		
		closeHttpConn(pool_conn->m_sock);
		pool_conn->close();
		return;
	}

	if (req) {
		m_request_hdl(http_conn, req);
		if (!http_conn->alive || http_conn->closing) {
			closeHttpConn(pool_conn->m_sock);
			pool_conn->close();
		}
	}
}
*/
void HttpSrv::start(int port) {
	
	m_poolserver->start(port);
}

