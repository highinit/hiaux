#include "HiLog.h"

namespace hiaux {

namespace log {

Log::Log(EvType _default_filter):
m_default_filter(_default_filter) {
	
}

Log::~Log() {
	
}

void Log::log (EvType _type, const std::string &_mess) {
	
}

void Log::log (EvType _filter, const ReqTrack &_reqinfo) {
	
}

}
}
