#include "MRInterMerger.h"

#include <unordered_map>

Int64VecPtr MRInterMerger::mergeKeys(Int64VecPtr a, Int64VecPtr b)
{
	Int64VecPtr keys_vec(new Int64Vec);
	
	std::unordered_map<int64_t, int> keys_map;
	
	for (int i = 0; i<a->size(); i++)
	{
		keys_map[ a->at(i) ] = 1;
	}
	
	for (int i = 0; i<b->size(); i++)
	{
		keys_map[ b->at(i) ] = 1;
	}
	
	auto it = keys_map.begin();
	auto keys_map_end = keys_map.end(); 
	
	while(it != keys_map_end)
	{
		keys_vec->push_back(it->first);
		it++;
	}
	keys_map.clear();
	
	return keys_vec;
}

void MRInterMerger::loadCache(MRInterResultPtr inter,
				bool cid,
				Int64VecPtr keys, int b, int e, hLock &lock)
{
	lock.lock();
	//std::cout << "loaded " << b << " to " << e << std::endl;
	for (int i = b; i<e; i++)
	{
		inter->preload(keys->at(i), cid);
	}
	inter->setCacheReady(cid);
	//std::cout << "loaded Cache\n";
	lock.unlock();
}

void MRInterMerger::merge(TaskLauncher &preload_tasks_launcher,
				MRInterResultPtr inter1,
				MRInterResultPtr inter2,
				MRInterResultPtr result,
				MapReduce *MR,
				int emits_in_cache)
{
	hLock inter1_lock;
	hLock inter2_lock;
	Int64VecPtr keys = mergeKeys(inter1->getKeys(), inter2->getKeys());
	//std::cout << "merge keys: " << keys->size() << std::endl;
	//const int nparts = 1000;
	int nemits = keys->size();
	int emits_per_part = fmin(emits_in_cache, nemits);//nemits/nparts;
	std::cout << "emits_per_part: " << emits_per_part << std::endl;
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
	
	//std::cout << "init caches loaded\n";
	int nread = 0;
	while (1)
	{	
		//std::cout << "waiting cache\n";
		inter1->condWaitCache(cid);
		inter2->condWaitCache(cid);
		
		//std::cout << "reading from cache " << key_r_b << " to " << key_r_e << "\n";
		nread += key_r_e - key_r_b;
		// get
		
		for (int i = key_r_b; i<key_r_e; i++)
		{
			EmitType* emit1 = inter1->getEmit(keys->at(i), cid);
			EmitType* emit2 = inter2->getEmit(keys->at(i), cid);
			
			if (emit1 == NULL && emit2 == NULL)
			{
			
			}	
			else if (emit1 == NULL)
			{
				//std::cout << "- " << emit2->key << std::endl;
				result->addEmit(keys->at(i), emit2);
			}
			else if (emit2 == NULL)
			{
				//std::cout << emit1->key <<  " -" << std::endl;
				result->addEmit(keys->at(i), emit1);
			}
			else
			{
				//std::cout << emit1->key <<  " " << emit2->key << std::endl;
				result->addEmit(keys->at(i), MR->reduce(keys->at(i), 
														emit1,
														emit2) );
			}	
		}
		
		key_r_b += emits_per_part;
		key_r_e = fmin(key_r_b+emits_per_part, nemits);
	
		if (key_r_b >= nemits)
		{
			//std::cout << "read all: " << nread << std::endl;
		//	exit(0);
			break;
		}
	
		key_b += emits_per_part;
		key_e = fmin(key_b+emits_per_part, nemits);
		
		inter1->clearCache(cid);
		inter2->clearCache(cid);
		preload_tasks_launcher.addTask(
			new boost::function<void()>( boost::bind(&MRInterMerger::loadCache,
								inter1,
								cid,
								keys,
								key_b,
								key_e,
								inter1_lock)));
		preload_tasks_launcher.addTask(
			new boost::function<void()>( boost::bind(&MRInterMerger::loadCache,
								inter2,
								cid,
								keys,
								key_b,
								key_e,
								inter2_lock)));
		cid = !cid;
	}
}
