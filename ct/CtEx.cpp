#include "CtEx.h"

CtEx::CtEx(const std::string &_mess):
m_mess(_mess) {
}

CtEx::CtEx() {
}

CtEx::~CtEx() throw() {
	
}

const char* CtEx::what() {
	
	return m_mess.c_str();
}

