#include "HiApiClientA.h"

HiApiClientA::HiApiClientA(Mode _mode, const std::string &_endpoint):
	 m_mode(_mode),
	 m_keyset(false),
	 m_endpoint(_endpoint) {
	
}

HiApiClientA::HiApiClientA(Mode _mode, const std::string &_endpoint, const std::string &_user, const std::string &_key):
	m_mode(_mode),
	m_user(_user),
	m_key(_key),
	m_keyset(false),
	m_endpoint(_endpoint) {
	
}

HiApiClientA::~HiApiClientA() {
	
}

void HiApiClientA::call (const std::string &_method, std::map<std::string, std::string> &_params, boost::function<void(bool, const std::string &)> &_onFinished) {
	
}

void HiApiClientA::callSigned (const std::string &_method, std::map<std::string, std::string> &_params, boost::function<void(bool, const std::string &)> &_onFinished) {
	
}
