#include "tasklauncher.h"

class BatchLauncher {

	TaskLauncher m_task_launcher;

	std::vector< boost::function<void()> > m_tasks;
	hAutoLock m_tasks_lock;
	hCondWaiter cond_on_finished;
	
	bool m_finished;
	
public:
	
	BatchLauncher(hThreadPool *_pool, size_t nthreads);
	
	bool checkFinished();
	void onFinished();
	TaskLauncher::TaskRet onLaunch(size_t _taskid);
	
	void addTask(boost::function<void()> _task);
	void setNoMoreTasks();
	void waitFinish();
};
