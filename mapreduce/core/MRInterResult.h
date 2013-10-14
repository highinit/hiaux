#ifndef MRNODE_DISPATCHER
#define MRNODE_DISPATCHER

#include "mapreduce.h"
#include "../../threadpool/threadpool.h"

class MRInterResult
{
	int m_fd;
	EmitDumper* m_dumper;
	// key, offset
	std::unordered_map<int64_t, off_t> m_file_map;
	
	std::unordered_map<int64_t, EmitType*> m_emit_cache0;
	std::unordered_map<int64_t, EmitType*> m_emit_cache1;
	
	bool m_cache0_ready;
	bool m_cache1_ready;
	hCondWaiter m_cache0_ready_lock;
	hCondWaiter m_cache1_ready_lock;
	
	// not thread safe
	EmitType *restore(off_t offset);
	
public:
	
	MRInterResult(int fd, EmitDumper* dumper);
	
	bool checkCacheReady(bool cid);
	
	// add to m_file_map and dump. not thread safe with itself and other methods
	void addEmit(int64_t key, EmitType *emitvec);
	std::vector<int64_t> getKeys();
	
	// preload & getEmit thread safe when cid's are different
	void preload(int64_t key, bool cid); 
	void condWaitCache(bool cid);
	// get emit from cache
	EmitType* getEmit(int64_t key, bool cid);
	
	void clearCache(bool cid);
	void setCacheReady(bool cid);
};

#endif

/*
 *  f (keys, nkeys, i1, i2, cid)				|		preload(keys, i1, i2, cid)
 *		addTask preload(keys[nkeys..2*nkeys], i1, i2, !cid)
 *		i1.condWaitCache(cid)
 *		i2.condWaitCache(cid)
 *		merge (keys[0..nkeys], i1, i2)
 *		i1.clearCache(cid)
 *		i2.clearCache(cid)
 *		f (keys[nkeys..], nkeys, i1, i2, !cid)
 * 
 * 
 * 
 */