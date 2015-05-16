#ifndef _CT_EXCEPTION_H_
#define _CT_EXCEPTION_H_

#include <exception>
#include <string>

class CtEx : std::exception {
	std::string m_mess;
public:
	
	CtEx(const std::string &_mess);
	CtEx();
	virtual ~CtEx() throw ();
	virtual const char* what();
};

#endif
