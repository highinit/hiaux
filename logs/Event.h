#ifndef _HILOG_EVENT_
#define _HILOG_EVENT_

#include "hiconfig.h"
#include <string>
#include <stdint.h>

namespace hiaux {

namespace log {

typedef uint16_t EvType;
typedef uint64_t HiTs;

#define ALL -1
#define BUISNESS 0
#define TRACE 1
#define DEBUG 2
#define INFO 3
#define WARNING 4
#define ERROR 5
#define FATAL 6

class Event {

	HiTs m_ts;
	EvType m_type;
	std::string m_mess;

public:
	
	Event(HiTs _ts, EvType _type, const std::string &_mess);
	virtual ~Event();
	
};
	
}

}

#endif
