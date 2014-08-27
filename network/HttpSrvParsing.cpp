#include "HttpSrv.h"

int HttpSrv_onMessageBegin(http_parser* parser) {
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	return conn->onMessageBegin();
}

int HttpSrv_onUrl(http_parser* parser, const char *at, size_t length) {
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	return conn->onUrl(at, length);
}

int HttpSrv_onStatus(http_parser* parser, const char *at, size_t length) {
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	return conn->onStatus(at, length);
}

int HttpSrv_onHeadersField(http_parser* parser, const char *at, size_t length) {
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	return conn->onHeadersField(at, length);
}

int HttpSrv_onHeadersValue(http_parser* parser, const char *at, size_t length) {
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	return conn->onHeadersValue(at, length);
}

int HttpSrv_onHeadersComplete(http_parser* parser) {
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	return conn->onHeadersComplete();
}

int HttpSrv_onBody(http_parser* parser, const char *at, size_t length) {
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	return conn->onBody(at, length);	
}

int HttpSrv_onMessageComplete(http_parser* parser) {
	HttpSrv::Connection* conn = (HttpSrv::Connection*)parser->data;
	return conn->onMessageComplete();
}

int HttpSrv::Connection::onMessageBegin() {
	//std::cout << "HttpSrv::Connection::onMessageBegin" << std::endl;
	return 0;
}

int HttpSrv::Connection::onUrl(const char *at, size_t length) {

	m_cur_request.url = std::string(at);
	m_cur_request.url = m_cur_request.url.substr(0, m_cur_request.url.find(' '));
	//fix_utf8_string(cur_request.url);
	m_cur_request.path = getUrlPath(m_cur_request.url);
	parseGET(m_cur_request.url, m_cur_request.values_GET);
	return 0;
}

int HttpSrv::Connection::onStatus(const char *at, size_t length) {
	//std::cout << "HttpSrv::Connection::onStatus" << at << std::endl;
	return 0;
}

int HttpSrv::Connection::onHeadersField(const char *at, size_t length) {
	char bf[length+1];
	strncpy(bf, at, length);
	bf[length] = '\0';
	m_cur_header_field = std::string(bf);
	//std::cout << "HttpSrv::Connection::onHeadersField: " << bf << std::endl;
	return 0;
}

int HttpSrv::Connection::onHeadersValue(const char *at, size_t length) {
	char bf[length+1];
	strncpy(bf, at, length);
	bf[length] = '\0';
	//std::cout << "HttpSrv::Connection::onHeadersValue: " << bf << std::endl;
	if (m_cur_header_field == "Cookie") {
		parseCookies(bf, m_cur_request.cookies);
	}
	
	return 0;
}

int HttpSrv::Connection::onHeadersComplete() {
	//std::cout << "HttpSrv::Connection::onHeadersComplete" << std::endl;
	return 0;
}

int HttpSrv::Connection::onBody(const char *at, size_t length) {

//	std::cout << "HttpSrv::Connection::onBody size:" << length << std::endl;
	m_cur_request.body = std::string(at);
//	std::cout << "body: " << m_cur_request.body << std::endl;
//	fix_utf8_string(m_cur_request.body);
	return 0;
}

int HttpSrv::Connection::onMessageComplete() {
	//std::cout << "HttpSrv::Connection::onMessageComplete" << std::endl;
	//requests.push( RequestPtr(new Request( cur_request ) ) );
	m_onRequest(m_sock, RequestPtr(new Request( m_cur_request ) ) );
	return 0;
}

