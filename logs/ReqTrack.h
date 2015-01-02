#ifndef _HILOG_REQTRACK_H_
#define _HILOG_REQTRACK_H_

#include "hiconfig.h"
#include <string>
#include <vector>
#include "Event.h"

#include <boost/shared_ptr.hpp>

namespace hiaux {

namespace log {

class ReqTrack {

	std::string m_reqid;
	std::vector<Event> m_events;

public:
	
	ReqTrack(const std::string &_reqid);
	virtual ~ReqTrack();
	
	void log (EvType _type, const std::string &_mess);
};
	
typedef boost::shared_ptr<ReqTrack> ReqTrackPtr;

}
}

#endif
