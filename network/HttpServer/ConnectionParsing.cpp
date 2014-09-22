#include "Connection.h"

int HttpConnection_onMessageBegin(http_parser* parser) {
	HttpConnection* conn = (HttpConnection*)parser->data;
	return conn->onMessageBegin();
}

int HttpConnection_onUrl(http_parser* parser, const char *at, size_t length) {
	HttpConnection* conn = (HttpConnection*)parser->data;
	return conn->onUrl(at, length);
}

int HttpConnection_onStatus(http_parser* parser, const char *at, size_t length) {
	HttpConnection* conn = (HttpConnection*)parser->data;
	return conn->onStatus(at, length);
}

int HttpConnection_onHeadersField(http_parser* parser, const char *at, size_t length) {
	HttpConnection* conn = (HttpConnection*)parser->data;
	return conn->onHeadersField(at, length);
}

int HttpConnection_onHeadersValue(http_parser* parser, const char *at, size_t length) {
	HttpConnection* conn = (HttpConnection*)parser->data;
	return conn->onHeadersValue(at, length);
}

int HttpConnection_onHeadersComplete(http_parser* parser) {
	HttpConnection* conn = (HttpConnection*)parser->data;
	return conn->onHeadersComplete();
}

int HttpConnection_onBody(http_parser* parser, const char *at, size_t length) {
	HttpConnection* conn = (HttpConnection*)parser->data;
	return conn->onBody(at, length);	
}

int HttpConnection_onMessageComplete(http_parser* parser) {
	HttpConnection* conn = (HttpConnection*)parser->data;
	return conn->onMessageComplete();
}

int HttpConnection::onMessageBegin() {

	return 0;
}

int HttpConnection::onUrl(const char *at, size_t length) {

	char bf[length+1];
	memcpy(bf, at, length);
	bf[length] = '\0';
	request->url = std::string(at);
	request->url = request->url.substr(0, request->url.find(' '));
	request->path = getUrlPath(request->url);
	parseGET(request->url, request->values_GET);
	return 0;
}

int HttpConnection::onStatus(const char *at, size_t length) {
	return 0;
}

int HttpConnection::onHeadersField(const char *at, size_t length) {
	
	char bf[length+1];
	memcpy(bf, at, length);
	bf[length] = '\0';
	m_cur_header_field = std::string(bf);
	return 0;
}

int HttpConnection::onHeadersValue(const char *at, size_t length) {
	
	char bf[length+1];
	memcpy(bf, at, length);
	bf[length] = '\0';

	if (m_cur_header_field == "Cookie") {
		parseCookies(bf, request->cookies);
	}
	
	return 0;
}

int HttpConnection::onHeadersComplete() {

	return 0;
}

int HttpConnection::onBody(const char *at, size_t length) {

	char bf[length+1];
	memcpy(bf, at, length);
	bf[length] = '\0';
	request->body = std::string(bf);
	
	return 0;
}

int HttpConnection::onMessageComplete() {

	request_finished = true;
	return 0;
}
