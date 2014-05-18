#include "batchlauncher.h"

/*
BatchLauncher::BatchLauncher(hThreadPool *_pool, size_t _maxparallel):
	 m_task_launcher(_pool, _maxparallel, boost::bind(&BatchLauncher::onFinished, this)),
	 m_finished(false),
	 cond_on_finished(boost::bind(&BatchLauncher::checkFinished, this)) {
}

void BatchLauncher::onFinished() {
	m_finished = true;
	cond_on_finished.kick();
}

bool BatchLauncher::checkFinished() {
	return m_finished;
}

TaskLauncher::TaskRet BatchLauncher::onLaunch(size_t _taskid) {
	boost::function<void()> task;
	{
		hLockTicketPtr ticket = m_tasks_lock.lock();
		task = m_tasks[_taskid];
	}
	task();
	return TaskLauncher::NO_RELAUNCH;
}

void BatchLauncher::addTask(boost::function<void()> _task) {
	hLockTicketPtr ticket = m_tasks_lock.lock();
	m_tasks.push_back(_task);
	m_task_launcher.addTask( new boost::function<TaskLauncher::TaskRet()> (
		boost::bind(&BatchLauncher::onLaunch, this, m_tasks.size()-1 ) ) );
}

void BatchLauncher::setNoMoreTasks() {
	m_task_launcher.setNoMoreTasks();
}

void BatchLauncher::waitFinish() {
	cond_on_finished.wait();
}
*/