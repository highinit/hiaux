#include "Event.h"

namespace hiaux {

namespace log {

Event::Event(LogTs _ts, EvType _type, const std::string &_method, const std::string &_mess):
m_ts(_ts),
m_type(_type),
m_method(_method),
m_mess(_mess) {
	
}

Event::~Event() {
	
}

std::string Event::asTsv() const {
	
	return inttostr(m_ts) + "	" + inttostr(m_type) + "	" + m_method + "	" + m_mess;
}

}
}
