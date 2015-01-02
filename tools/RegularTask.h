#include "hiconfig.h"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace hiaux {

class RegularTask {

	boost::function<void()> m_task;
	uint64_t m_last_launch_ts;
	uint64_t m_period;

public:
	
	RegularTask(uint64_t _period, boost::function<void()> _task);
	virtual ~RegularTask();
	
	void checkRun();
};

typedef boost::shared_ptr<RegularTask> RegularTaskPtr;

}
