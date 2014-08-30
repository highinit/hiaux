#include "RefParser.h"

std::map<std::string, std::string> RefParser::m_engines;
RefParser::Initer RefParser::initer;

RefParser::Initer::Initer() {
	
	RefParser::init();
}

void RefParser::init() {
	
	m_engines["bing.com/search"] = "q=";
	m_engines["ask.com/search/"] = "searchfor=";
	m_engines["google.ru"] = "q=";
	m_engines["mail.ru/search"] = "q=";
	m_engines["nigma.ru"] = "s=";
	m_engines["qip.ru/search"] = "query=";
	m_engines["rambler.ru"] = "query=";
	m_engines["sputnik.ru"] = "q=";
	m_engines["webalta.ru/search"] = "q=";
	m_engines["yahoo.com/search"] = "p=";
	m_engines["yandex."] = "text="; // !!! etext
}

bool RefParser::parseEngineReferer(const std::string &_ref, const std::string &_engine, const std::string &_param, std::string &_query) {
	
	if (_ref.find(_engine) == -1)
		return false;
	
	int bpos = _ref.find(_param) + _param.size();
	
	if (bpos == -1 + _param.size())
		return false;
	
	_query = _ref.substr(bpos, _ref.size()-bpos);
	int epos = _query.find("&");
	
	if (epos == -1)
		epos = _query.size();
	
	_query = _query.substr(0, epos);
	unescapeUrl(_query);
	toLowerUtf8(_query);
	replaceCharUtf8(_query, 0x2B, 0x20); // '+' to space 
	
	return true;
}

bool RefParser::parse(const std::string &_ref, std::string &_query) {
	
	std::map<std::string, std::string>::const_iterator it = m_engines.begin();
	std::map<std::string, std::string>::const_iterator end = m_engines.end();
	
	std::string ref(_ref);
	unescapeUrl(ref);
	
	while (it != end) {
		if (parseEngineReferer(ref, it->first, it->second, _query))
			return true;
		it++;
	}
	return false;
}
