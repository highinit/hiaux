#include "MRInterResult.h" 

#include <sys/types.h>
#include <dirent.h>

#include <sys/uio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/uio.h> 
#include <unistd.h>

MRInterResult::MRInterResult(std::string filename,
							EmitDumper* dumper,
							TaskLauncher &flush_launcher,
							const size_t max_buffer_size):
	m_dumper(dumper),
	m_cache0_ready_lock(boost::bind(&MRInterResult::checkCacheReady, this, 0)),
	m_cache1_ready_lock(boost::bind(&MRInterResult::checkCacheReady, this, 1)),
	no_more_writes(0),
	m_max_buffer_size(max_buffer_size),
	flush_finished(0),
	flush_finish_lock(boost::bind(&MRInterResult::FlushFinished, this))
{
	m_fd = open(filename.c_str(),  O_RDWR | O_CREAT | O_TRUNC,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	flush_launcher.addTask(new boost::function<bool()>(
		boost::bind(&MRInterResult::flushBuffer, this)));
}

MRInterResult::~MRInterResult()
{
	close(m_fd);
	m_file_map.clear();
	m_emit_cache0.clear();
	m_emit_cache1.clear();
}

bool MRInterResult::checkWriteBufferNotEmpty()
{
	wbuffer_lock.lock();
	bool notempty = (write_buffer.size()==0);
	wbuffer_lock.lock();
	return notempty;
}

bool MRInterResult::FlushFinished()
{
	return flush_finished;
}

void MRInterResult::flush(std::pair<int64_t, std::string> dump)
{
	off_t offset = lseek(m_fd, 0, SEEK_END);
	size_t size = dump.second.size();

	iovec atom[3];
	atom[0].iov_base =  &dump.first;
	atom[0].iov_len = sizeof(int64_t);

	atom[1].iov_base =  &size;
	atom[1].iov_len = sizeof(size_t);

	atom[2].iov_base = (void*)dump.second.data();
	atom[2].iov_len = size;

	writev(m_fd, atom, 3);
	dump.second.clear();
	
	m_file_map.insert(std::pair<int64_t, off_t>(dump.first, offset));
}

bool MRInterResult::flushBuffer()
{
	//std::cout << ".";
	//std::cout << "MRInterResult::flushBuffer wbuffer_lock.lock ";
	wbuffer_lock.lock();
	//std::cout << "OK\n";

	if (write_buffer.size()>m_max_buffer_size)
	{
		while (write_buffer.size()>m_max_buffer_size/2)
		{
			flush(write_buffer.front());
			write_buffer.pop();
		}
	}
	else if (write_buffer.size()!=0)
	{
		flush(write_buffer.front());
		write_buffer.pop();
	}
	else
	{
		if (no_more_writes)
		{
			wbuffer_lock.unlock();
			
			flush_finish_lock.lock();
			flush_finished = 1;
			flush_finish_lock.kick();
			flush_finish_lock.unlock();
	
			return 0; // dont repeat
		}
	}
	wbuffer_lock.unlock();
	return 1; // repeat
}

void MRInterResult::waitFlushFinished()
{
	//std::cout << "MRInterResult::wbuffer_lock ";
	wbuffer_lock.lock();
	//std::cout << "OK\n";
	
	no_more_writes = 1;
	/*if (write_buffer.size()==0)
	{
		flush_finished = 1;
		wbuffer_lock.unlock();
		return;
	}*/
	wbuffer_lock.unlock();
	
//	std::cout << "MRInterResult::flush_finish_lock.wait ";
	flush_finish_lock.wait();
	//std::cout << "OK\n";
}

bool MRInterResult::checkCacheReady(bool cid)
{
	if (cid==false)
	{
		return m_cache0_ready.load();
	}
	else
	{
		return m_cache1_ready.load();
	}
}

void MRInterResult::addEmit(int64_t key, EmitType *emit)
{
	std::string dump = m_dumper->dump(emit);
	delete emit;

	//m_write_buffer_empty.lock();
	wbuffer_lock.lock();
	write_buffer.push(std::pair<int64_t, std::string>(key, dump));
	wbuffer_lock.unlock();
	//m_write_buffer_empty.kick();
	//m_write_buffer_empty.unlock();
}

EmitType *MRInterResult::restore(off_t offset)
{
	lseek(m_fd, offset, SEEK_SET);
	size_t size;
	int64_t key;
	if (read(m_fd, &key, sizeof(int64_t))!=sizeof(int64_t))
	{
		throw "MRInterResult::preload: READ ERROR\n";
	}
	
	if (read(m_fd, &size, sizeof(size_t))!=sizeof(size_t))
	{
		throw "MRInterResult::preload: READ ERROR\n";
	}
	char bf[size+1];
	if (read(m_fd, bf, size)!=size)
	{
		throw "MRInterResult::preload: READ ERROR\n";
	}
	bf[size] = '\0';
	
	EmitType *emit = m_dumper->restore(bf);
	return emit;
}

void MRInterResult::preload(int64_t key, bool cid)
{
	auto it = m_file_map.find(key);
	if (it==m_file_map.end())
	{
		//std::cout << "no key in filemap\n";
		//exit(0);
		return;
		//throw "MRInterResult::preload: No such key";
	}
	
	EmitType *emit = restore(it->second);
	
	if (cid==false)
	{
		m_emit_cache0.insert(std::pair<int64_t, EmitType*>(key, emit));
	}
	else
	{
		m_emit_cache1.insert(std::pair<int64_t, EmitType*>(key, emit));
	}
}

EmitType* MRInterResult::getEmit(int64_t key, bool cid)
{
	if (cid==false)
	{
		std::unordered_map<int64_t, EmitType*>::iterator cache_it = m_emit_cache0.find(key);

		if (cache_it != m_emit_cache0.end())
		{
			EmitType *emit = cache_it->second;
			m_emit_cache0.erase(cache_it);
			return emit;
		}
		else
		{
			return NULL;
			//throw "MRInterResult::getEmit Not in cache";
		}
	}
	else
	{
		std::unordered_map<int64_t, EmitType*>::iterator cache_it = m_emit_cache1.find(key);

		if (cache_it != m_emit_cache1.end())
		{
			EmitType *emit = cache_it->second;
			m_emit_cache1.erase(cache_it);
			return emit;
		}
		else
		{
			//throw "MRInterResult::getEmit Not in cache";
			return NULL;
		}
	}
}

void MRInterResult::condWaitCache(bool cid)
{
	if (!cid)// && !m_cache0_ready.load())
	{
		m_cache0_ready_lock.wait();
	}
	else if (cid)// && !m_cache1_ready.load())
	{
		m_cache1_ready_lock.wait();
	}
}

void MRInterResult::clearCache(bool cid)
{
	if (cid==false)
	{
		m_cache0_ready_lock.lock();
		m_cache0_ready = 0;
		m_cache0_ready_lock.unlock();
	}
	else
	{
		m_cache1_ready_lock.lock();
		m_cache1_ready = 0;
		m_cache1_ready_lock.unlock();
	}
}

void MRInterResult::setCacheReady(bool cid)
{
	if (cid==false)
	{
		m_cache0_ready_lock.lock();
		m_cache0_ready = 1;
		m_cache0_ready_lock.kick();
		m_cache0_ready_lock.unlock();
	}
	else
	{
		m_cache1_ready_lock.lock();
		m_cache1_ready = 1;
		m_cache1_ready_lock.kick();
		m_cache1_ready_lock.unlock();
	}
}

Int64VecPtr MRInterResult::getKeys()
{
	Int64VecPtr keys( new std::vector<int64_t> );
	auto filemap_it = m_file_map.begin();
	auto filemap_end = m_file_map.end();
	
	while (filemap_it != filemap_end)
	{
		keys->push_back(filemap_it->first);
		filemap_it++;
	}
	return keys;
}
