#include "MRInterMerger.h"

#include <tr1/unordered_map>

Int64VecPtr MRInterMerger::mergeKeys(Int64VecPtr a, Int64VecPtr b)
{
//	std::cout << "merge keys: " << a->size() << ", " << b->size() << std::endl;
	Int64VecPtr keys_vec(new Int64Vec);

	std::tr1::unordered_map<uint64_t, int> keys_map;

	for (int i = 0; i<a->size(); i++)
	{
		keys_map[ a->at(i) ] = 1;
	}
	
	for (int i = 0; i<b->size(); i++)
	{
		keys_map[ b->at(i) ] = 1;
	}

	std::tr1::unordered_map<uint64_t, int>::iterator it = keys_map.begin();
	std::tr1::unordered_map<uint64_t, int>::iterator keys_map_end = keys_map.end(); 
	
	while(it != keys_map_end)
	{
		keys_vec->push_back(it->first);
		it++;
	}
	keys_map.clear();

	return keys_vec;
}

TaskLauncher::TaskRet MRInterMerger::loadCache(MRInterResultPtr inter,
				bool cid,
				Int64VecPtr keys, int b, int e, hLock &lock)
{
	lock.lock();
	for (int i = b; i<e; i++)
	{
		inter->preload(keys->at(i), cid);
	}
	inter->setCacheReady(cid);
	lock.unlock();
	return TaskLauncher::NO_RELAUNCH;
}

MRStats MRInterMerger::merge(TaskLauncher &preload_tasks_launcher,
				MRInterResultPtr inter1,
				MRInterResultPtr inter2,
				MRInterResultPtr result,
				MapReduce *MR,
				int emits_in_cache)
{
	MRStats stats;
	hLock inter1_lock;
	hLock inter2_lock;
	Int64VecPtr keys = mergeKeys(inter1->getKeys(), inter2->getKeys());

	int nemits = keys->size();
	int emits_per_part = fmin(emits_in_cache, nemits);
	int key_r_b = 0;
	int key_r_e = emits_per_part;

	bool cid = 0;
	int key_b = 0;
	int key_e = fmin(key_b+emits_per_part, nemits);

	loadCache(inter1, 0, keys, key_b, key_e, inter1_lock);
	loadCache(inter2, 0, keys, key_b, key_e, inter2_lock);

	key_b += emits_per_part;
	key_e = fmin(key_b+emits_per_part, nemits);

	loadCache(inter1, 1, keys, key_b, key_e, inter1_lock);
	loadCache(inter2, 1, keys, key_b, key_e, inter2_lock);

	int nread = 0;
	while (1)
	{	
		inter1->condWaitCache(cid);
		inter2->condWaitCache(cid);
		
		nread += key_r_e - key_r_b;
		
		for (int i = key_r_b; i<key_r_e; i++)
		{
			EmitType* emit1 = inter1->getEmit(keys->at(i), cid);
			EmitType* emit2 = inter2->getEmit(keys->at(i), cid);
			stats.nreduces++;
			if (emit1 == NULL && emit2 == NULL)
			{
			
			}	
			else if (emit1 == NULL)
			{
				result->addEmit(keys->at(i), emit2);
			}
			else if (emit2 == NULL)
			{
				result->addEmit(keys->at(i), emit1);
			}
			else
			{
				result->addEmit(keys->at(i), MR->reduce(keys->at(i),
										emit1,
										emit2) );
			}
		}

		key_r_b += emits_per_part;
		key_r_e = fmin(key_r_b+emits_per_part, nemits);

		if (key_r_b >= nemits)
		{
			break;
		}

		key_b += emits_per_part;
		key_e = fmin(key_b+emits_per_part, nemits);

		inter1->clearCache(cid);
		inter2->clearCache(cid);

		if (key_b<nemits)
		{
			preload_tasks_launcher.addTask(
				new boost::function<TaskLauncher::TaskRet()>( boost::bind(&MRInterMerger::loadCache,
									inter1,
									cid,
									keys,
									key_b,
									key_e,
									inter1_lock)));
			preload_tasks_launcher.addTask(
				new boost::function<TaskLauncher::TaskRet()>( boost::bind(&MRInterMerger::loadCache,
									inter2,
									cid,
									keys,
									key_b,
									key_e,
									inter2_lock)));
		}
		cid = !cid;
	}
	return stats;
}
