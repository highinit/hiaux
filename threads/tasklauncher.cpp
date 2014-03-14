#include "tasklauncher.h"

TaskLauncher::TaskLauncher(hThreadPool *pool,
						size_t max_parallel,
						boost::function<void()> onFinished):
	m_pool(pool),
	tasks_launched(0),
	tasks_finished(0),
	all_tasks_launched(0),
	all_tasks_finished(0),
	m_max_parallel(max_parallel),
	no_more_tasks(0),
	m_onFinished(onFinished)
{	
}

void TaskLauncher::Task(boost::function<TaskLauncher::TaskRet()> *task)
{
	TaskLauncher::TaskRet ret = (*task)();
	
	if (ret == RELAUNCH)
	{
		addTask(task);
	}
	else
	{
		delete task;
	}
	
	tasks_finished++;
	checkLaunch();
	checkFinished();
}

void TaskLauncher::launch(boost::function<TaskLauncher::TaskRet()> *task)
{
	tasks_launched++;
	m_pool->addTask(new boost::function<void()>(boost::bind(&TaskLauncher::Task, this, task)));
}

void TaskLauncher::checkLaunch()
{
	while (countRunning() < m_max_parallel)
	{	
		boost::function<TaskLauncher::TaskRet()>* f;
		task_q.lock();
		if (task_q.size()!=0)
		{
			f = task_q.front();
			task_q.pop();
			task_q.unlock();
			launch (f);
		}
		else
		{
			task_q.unlock();
			break;
		}
	}
}

void TaskLauncher::addTask(boost::function<TaskLauncher::TaskRet()> *task)
{
	task_q.lock();
	task_q.push(task);
	task_q.unlock();
	checkLaunch();
}

void TaskLauncher::incLaunched()
{
	tasks_launched++;
}

void TaskLauncher::checkFinished()
{
	if (tasks_launched.load() == tasks_finished.load() && no_more_tasks.load() && task_q.empty())
	{
	//	std::cout << "______TaskLauncher::finished\n";
		if (finish_lock.trylock())
		{
			if (all_tasks_finished.load()) return;
			all_tasks_finished = 1;
			m_onFinished();
			finish_lock.unlock();
		}
	}
}

void TaskLauncher::setMaxParallel(size_t max_parallel)
{
	m_max_parallel = max_parallel;
}

void TaskLauncher::setNoMoreTasks()
{
	no_more_tasks = 1;
}

size_t TaskLauncher::countRunning()
{
	return tasks_launched.load() - tasks_finished.load();
}

size_t TaskLauncher::countLaunched()
{
	return tasks_launched.load();
}

size_t TaskLauncher::countFinished()
{
	return tasks_finished.load();
}

bool TaskLauncher::checkAllLaunched()
{
	return all_tasks_launched.load();
}
