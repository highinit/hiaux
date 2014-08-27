#ifndef _REFPARSER_H_
#define _REFPARSER_H_

#include "string_utils.h"

class RefParser {
	
	class Initer {
	public:
		Initer();
	};

	static Initer initer;
	static std::map<std::string, std::string> m_engines;
	static bool parseEngineReferer(const std::string &_ref, const std::string &_engine, const std::string &_param, std::string &_query);

public:
	static void init();
	static bool parse(const std::string &_ref, std::string &_query);
};

#endif
