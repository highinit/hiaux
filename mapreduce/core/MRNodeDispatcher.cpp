#include "MRNodeDispatcher.h"

vomerledorg

{

};

void MRNodeDispatcher::reduceTask(MRInterResultPtr a, MRInterResultPtr b)
{
	
}

void MRNodeDispatcher::onGotResult(MRInterResultPtr inter_result)
{
	inter_results.lock();
	inter_results.push(inter_result);
	
	while (inter_results.size()>1)
	{
		MRInterResultPtr a = inter_results.front();
		inter_results.pop();
		MRInterResultPtr b = inter_results.front();
		inter_results.pop();
		
		reduce_tasks_launcher.addTask(new boost::bind(&MRNodeDispatcher::reduceTask, this, a, b);
	}
	
	inter_results.unlock();
}
