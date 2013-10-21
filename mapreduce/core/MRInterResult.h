#ifndef MRINTER_RESULT_DISPATCHER
#define MRINTER_RESULT_DISPATCHER

#include "mapreduce.h"
#include "../../threadpool/threadpool.h"
#include "../../threadpool/tasklauncher.h"
#include "../core/InterResultReader.h"

typedef std::vector<uint64_t> Int64Vec;
typedef boost::shared_ptr< std::vector<uint64_t> > Int64VecPtr;

#define IR_WRITING 0
#define IR_READING 1

class MRInterResult
{
	bool mode;
	
	std::string m_filename;
	int m_fd;
	EmitDumperPtr m_dumper;
	InterResultLoader *m_reader;
	
	// key, offset
	std::unordered_map<uint64_t, uint64_t> m_file_map;
	
	std::unordered_map<uint64_t, EmitType*> m_emit_cache0;
	std::unordered_map<uint64_t, EmitType*> m_emit_cache1;
	
	std::atomic<bool> m_cache0_ready;
	std::atomic<bool> m_cache1_ready;
	hCondWaiter m_cache0_ready_lock;
	hCondWaiter m_cache1_ready_lock;
	
	// key / dump
	//std::queue< std::pair<int64_t, std::string> > write_queue;
	boost::lockfree::queue< std::pair<uint64_t, std::string>* > write_queue;
	uint8_t *wbuffer;
	size_t m_wbuffer_size; // offset in wbuffer
	size_t m_wbuffer_cap;
	size_t w_offset; // offset in file
	
	//hLock wbuffer_lock;
	std::atomic<bool> no_more_writes;
	
	bool flush_finished; // buffer empty && nomore
	hCondWaiter flush_finish_lock;
	
	// not thread safe
	EmitType *restore(off_t offset);
	
	void flush(std::pair<uint64_t, std::string> dump);
	void flush_wbuffer();
public:
	
	MRInterResult(std::string filename,
				EmitDumperPtr dumper,
				TaskLauncher &flush_launcher,
				const size_t wbuffer_cap = 50000000);
	
	~MRInterResult();
	
	//bool checkWriteBufferNotEmpty();
	bool flushBuffer();
	
	bool checkCacheReady(bool cid);
	bool FlushFinished();
	
	// add to m_file_map and dump. not thread safe with itself and other methods
	void addEmit(uint64_t key, EmitType *emitvec);
	void waitFlushFinished();
	Int64VecPtr getKeys();
	
	// preload & getEmit thread safe when cid's are different
	void preload(uint64_t key, bool cid); 
	void condWaitCache(bool cid);
	// get emit from cache
	EmitType* getEmit(uint64_t key, bool cid);
	
	void clearCache(bool cid);
	void setCacheReady(bool cid);
};

typedef boost::shared_ptr<MRInterResult> MRInterResultPtr;

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