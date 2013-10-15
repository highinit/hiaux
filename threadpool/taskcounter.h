#include "threadpool.h"

typedef boost::lockfree::queue<boost::function<void()>*> TaskQueue;

class TaskLauncher
{
	hThreadPool *m_pool;
	TaskQueue task_q;
	
	std::atomic<size_t> tasks_launched;
	std::atomic<size_t> tasks_finished;
	std::atomic<bool> all_tasks_finished;
	std::atomic<bool> all_tasks_launched;
	std::atomic<bool> no_more_tasks;
	
	hLock finish_lock;
	
	size_t m_max_parallel;
	boost::function<void()> m_onFinished;
	
	void incLaunched();
	void incFinished();
	
	void checkLaunch();
	void launch(boost::function<void()> *task);
public:

	void Task(boost::function<void()> *task);
	
	TaskLauncher(hThreadPool *pool,
				size_t max_parallel, 
				boost::function<void()> onFinished);

	void addTask(boost::function<void()> *task);

	
	void setMaxParallel(size_t max_parallel);
	void setNoMoreTasks();

	size_t countRunning();
	size_t countLaunched();
	size_t countFinished();
	bool checkAllLaunched();
};
