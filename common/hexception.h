#ifndef HEXCEPTION_H
#define HEXCEPTION_H

#include <string>
#include <exception>  

class hException : public std::exception
{
	std::string mess;
	public:
	hException(std::string _mess): mess(_mess) {}
	
	hException(const char* _mess): mess(std::string(_mess)) {}
	
	virtual const char* what() throw()
	{
		return mess.c_str();
	}
	virtual ~hException() throw()
	{
	
	}
};

#endif
