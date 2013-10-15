#include "taskcounter.h"

TaskLauncher::TaskLauncher(hThreadPool *pool,
						size_t max_parallel,
						boost::function<void()> onFinished):
	task_q(100),
	m_onFinished(onFinished),
	tasks_launched(0),
	tasks_finished(0),
	all_tasks_launched(0),
	all_tasks_finished(0),
	m_max_parallel(max_parallel),
	no_more_tasks(0),
	m_pool(pool)
{
	
}

void TaskLauncher::Task(boost::function<void()> *task)
{
	//std::cout << "launching \n";
	(*task)();
	delete task;
	incFinished();
}

void TaskLauncher::launch(boost::function<void()> *task)
{
	//std::cout << "adding task to pool \n";
	tasks_launched++;
	m_pool->addTask(new boost::function<void()>(boost::bind(&TaskLauncher::Task, this, task)));
}

void TaskLauncher::checkLaunch()
{
//	std::cout << "checkLaunch\n";
//	std::cout << "countRunning: " << countRunning() << std::endl;
//	std::cout << "m_max_parallel: " << m_max_parallel << std::endl;
	while (countRunning() < m_max_parallel)
	{
		//std::cout << "_____________________\n";
		boost::function<void()>* f;
		if (task_q.pop(f))
		{
			
			launch (f);
		}
		else
		{
		//	std::cout << "could not pop\n";
			break;
		}
	}
	//std::cout << "finished checkLaunch\n";
}

void TaskLauncher::addTask(boost::function<void()> *task)
{
	//std::cout << "add task \n";
	task_q.push(task);
	checkLaunch();
}

void TaskLauncher::incLaunched()
{
	tasks_launched++;
}

void TaskLauncher::incFinished()
{
	tasks_finished++;

	checkLaunch();

	if (tasks_launched.load() == tasks_finished.load() && no_more_tasks.load() && task_q.empty())
	{
		//if (all_tasks_launched.load())
		{
			if (finish_lock.trylock())
			{
				if (all_tasks_finished.load()) return;
				all_tasks_finished = 1;
				m_onFinished();
				finish_lock.unlock();
			}
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
