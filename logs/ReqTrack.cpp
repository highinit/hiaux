#include "ReqTrack.h"

namespace hiaux {

namespace log {

ReqTrack::ReqTrack(const std::string &_reqid):
m_reqid(_reqid) {
	
}

ReqTrack::~ReqTrack() {
	
}

void ReqTrack::log (EvType _type, const std::string &_mess) {
	
	m_events.push_back(Event(time(0), _type, _mess));
}

}
}
