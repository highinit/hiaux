#ifndef HEXCEPTION_H
#define HEXCEPTION_H

class hExeption : public std::exception
{
	std::string mess;
	public:
	hExeption(std::string _mess): mess(_mess) {}

	virtual const char* what() const noexcept
	{
		return mess.c_str();
	}
};

#endif
