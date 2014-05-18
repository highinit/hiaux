#ifndef MRTASK_LAUNCHER_H
#define MRTASK_LAUNCHER_H


#include "threadpool.h"

//typedef boost::lockfree::queue<boost::function<bool()>*> TaskQueue;

class TaskLauncher
{
public:
	enum TaskRet
	{
		RELAUNCH,
		NO_RELAUNCH
	};
private:
	
	class TaskQueue: public std::queue<boost::function<TaskLauncher::TaskRet()>*>, public hLock
	{
	};
	
	hThreadPoolPtr m_pool;
	TaskQueue task_q;
	
	boost::atomic<size_t> tasks_launched;
	boost::atomic<size_t> tasks_finished;
	boost::atomic<bool> all_tasks_launched;
	boost::atomic<bool> all_tasks_finished;
	size_t m_max_parallel;
	boost::atomic<bool> no_more_tasks;
	
	hLock finish_lock;
	
	boost::function<void()> m_onFinished;
	
	void incLaunched();
	void checkFinished();
	
	void checkLaunch();
	void launch(boost::function<TaskLauncher::TaskRet()> *task);
public:

	void Task(boost::function<TaskLauncher::TaskRet()> *task);
	
	TaskLauncher(hThreadPoolPtr pool,
				size_t max_parallel, 
				boost::function<void()> onFinished);

	void addTask(boost::function<TaskLauncher::TaskRet()> *task);

	void setMaxParallel(size_t max_parallel);
	void setNoMoreTasks();

	size_t countRunning();
	size_t countLaunched();
	size_t countFinished();
	bool checkAllLaunched();
};

typedef boost::shared_ptr<TaskLauncher> TaskLauncherPtr;

#endif
