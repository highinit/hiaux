#ifndef _HILOG_EVENT_
#define _HILOG_EVENT_

#include "LogDefs.h"
#include <string>

namespace hiaux {

namespace log {

class Event {

public:

	LogTs m_ts;
	EvType m_type;
	std::string m_method;
	std::string m_mess;
	
	Event(LogTs _ts, EvType _type, const std::string &_method, const std::string &_mess);
	virtual ~Event();
	
	std::string asTsv() const;
};
	
}

}

#endif
