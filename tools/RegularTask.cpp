#include "RegularTask.h"

namespace hiaux {

RegularTask::RegularTask(uint64_t _period, boost::function<void()> _task):
m_last_launch_ts(0),
m_period(_period),
m_task(_task) {
	
}

RegularTask::~RegularTask() {
	
}

void RegularTask::checkRun() {
	
	uint64_t now = time(0);
	if (now - m_last_launch_ts > m_period) {
		
		m_task();
		m_last_launch_ts = time(0);
	}
}

}
