#ifndef _HILOG_H_
#define _HILOG_H_

#include "hiconfig.h"
#include "ReqTrack.h"

#include <boost/shared_ptr.hpp>

#include <string>

#include "Event.h"

namespace hiaux {

namespace log {

class Log {
	
	EvType m_default_filter;
public:
	
	Log(EvType _default_filter);
	virtual ~Log();
	
	void log (EvType _type, const std::string &_mess);
	void log (EvType _filter, const ReqTrack &_reqinfo);
	
};

typedef boost::shared_ptr<Log> LogPtr;

}
}

#endif
