#include "MRInterResult.h"

#include <sys/types.h>
#include <dirent.h>

#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>

#include <errno.h>

#include "../../common/hexception.h"

MRInterResult::MRInterResult(std::string filename,
							MapReduce *MR,
							TaskLauncher &flush_launcher,
							const size_t wbuffer_cap):
	m_MR(MR),
	m_cache0_ready_lock(boost::bind(&MRInterResult::checkCacheReady, this, 0)),
	m_cache1_ready_lock(boost::bind(&MRInterResult::checkCacheReady, this, 1)),
	no_more_writes(0),
	m_wbuffer_cap(wbuffer_cap),
	flush_finished(0),
	flush_finish_lock(boost::bind(&MRInterResult::FlushFinished, this)),
	mode(IR_WRITING),
	m_filename(filename),
	w_offset(0),
	m_size(0)
{
	wbuffer = new uint8_t [wbuffer_cap];
	m_wbuffer_size = 0;
	
	m_fd = open(filename.c_str(),  O_RDWR | O_CREAT | O_TRUNC,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	flush_launcher.addTask(new boost::function<TaskLauncher::TaskRet()>(
		boost::bind(&MRInterResult::flushBuffer, this)));
}

MRInterResult::~MRInterResult()
{
//	delete [] wbuffer;
//	m_file_map->clear();
	m_emit_cache0.clear();
	m_emit_cache1.clear();
}

void MRInterResult::deleteFile()
{
	m_reader->deleteFile();
}

bool MRInterResult::FlushFinished()
{
	return flush_finished;
}

/*
void MRInterResult::flush(std::pair<uint64_t, std::string> dump)
{
	off_t offset = lseek(m_fd, 0, SEEK_END);
	uint64_t size = dump.second.size();

	iovec atom[3];
	atom[0].iov_base =  &dump.first;
	atom[0].iov_len = sizeof(uint64_t);

	atom[1].iov_base =  &size;
	atom[1].iov_len = sizeof(uint64_t);

	atom[2].iov_base = (void*)dump.second.data();
	atom[2].iov_len = size;

	writev(m_fd, atom, 3);
	dump.second.clear();
	
	m_file_map.insert(std::pair<uint64_t, uint64_t>(dump.first, offset));
}*/

void MRInterResult::flush_wbuffer()
{
	if (m_wbuffer_size==0)
		return;
	
	iovec atom[1];
	atom[0].iov_base = wbuffer;
	atom[0].iov_len = m_wbuffer_size;
	if (writev(m_fd, atom, 1)!=m_wbuffer_size)
	{
		throw hException("flush_wbuffer:: cant writev"+std::string(strerror(errno)));
	}
	m_wbuffer_size = 0;
}

TaskLauncher::TaskRet MRInterResult::flushBuffer()
{
	write_queue.lock();
	while (!write_queue.empty())
	{				
		/*if (write_queue.empty())
		{
			break;
		}*/
		
		
		std::pair<uint64_t, std::string> kv = write_queue.front();
		write_queue.pop();
		write_queue.unlock();
				
		//KeyType key = kv->first;
		uint64_t key = kv.first;
		std::string dump = kv.second;
		uint64_t size = dump.size();


		/*
		if (sizeof(uint64_t)+key.size()+size > m_wbuffer_cap-m_wbuffer_size)
		{
			flush_wbuffer();
		}
		*/
		if (2*sizeof(uint64_t)+size > m_wbuffer_cap-m_wbuffer_size)
		{
			flush_wbuffer();
		}

		//std::cout << "k: " << key << " v: " << dump << std::endl;
		
		//memcpy((void*)((uint8_t*)wbuffer+m_wbuffer_size), key.data(), key.size());
		memcpy((void*)((uint8_t*)wbuffer+m_wbuffer_size), &key, sizeof(uint64_t));
		m_wbuffer_size += sizeof(uint64_t);//key.size();
		memcpy((void*)((uint8_t*)wbuffer+m_wbuffer_size), &size, sizeof(uint64_t));
		m_wbuffer_size += sizeof(uint64_t);
		memcpy((void*)((uint8_t*)wbuffer+m_wbuffer_size), dump.data(), size);
		m_wbuffer_size += size;

		//m_file_map.insert(std::pair<uint64_t, uint64_t>(key, w_offset));
		w_offset +=  sizeof(uint64_t)+sizeof(uint64_t)+size;
		write_queue.lock();
	}
	
	if (no_more_writes.load())
	{
		flush_wbuffer();
		//std::cout << "no more writes: " << m_filename.c_str() << std::endl;
		mode = IR_READING;
		::close(m_fd);
		delete [] wbuffer;

		//m_file_map.clear();
		
		flush_finish_lock.lock();
		flush_finished = 1;
		flush_finish_lock.kick();
		flush_finish_lock.unlock();
		
		write_queue.unlock();
		return TaskLauncher::NO_RELAUNCH;
	}

	write_queue.unlock();
	return TaskLauncher::RELAUNCH;
}

void MRInterResult::waitFlushFinished()
{
	no_more_writes = 1;
	//std::cout << "waiting flush finished\n";
	flush_finish_lock.wait();
	//std::cout << "flush finished\n";
}

void MRInterResult::waitInitReading()
{
	m_reader.reset(new InterResultLoader(m_filename, m_MR));
	m_file_map = m_reader->getFileMap();
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

void MRInterResult::addEmit(KeyType key, EmitType *emit)
{
	if (mode!=IR_WRITING)
	{
		throw hException("MRInterResult::addEmit error mode!=IR_WRITING");
	}
	std::string dump = m_MR->dumpEmit(emit);
	delete emit;

	write_queue.lock();
	m_size++;
	write_queue.push(std::pair<KeyType, std::string>(key, dump));
	write_queue.unlock();
}

EmitType *MRInterResult::restore(off_t offset)
{
	lseek(m_fd, offset, SEEK_SET);
	uint64_t size;
	KeyType key;
	if (read(m_fd, &key, sizeof(uint64_t))!=sizeof(uint64_t))
	{
		throw hException("MRInterResult::preload: READ ERROR\n");
	}
	
	if (read(m_fd, &size, sizeof(uint64_t))!=sizeof(uint64_t))
	{
		throw hException("MRInterResult::preload: READ ERROR\n");
	}
	char bf[size+1];
	if (read(m_fd, bf, size)!=size)
	{
		throw hException("MRInterResult::preload: READ ERROR\n");
	}
	bf[size] = '\0';

	EmitType *emit = m_MR->restoreEmit(bf);
	return emit;
}

void MRInterResult::preload(uint64_t key, bool cid)
{
	if (mode!=IR_READING)
	{
		throw hException("MRInterResult::preload error mode!=IR_READING");
	}
	FileMap::iterator it = m_file_map->find(key);
	if (it==m_file_map->end())
	{
		return;
	}
	
	EmitType *emit = m_reader->readEmit(it->second);
	//EmitType *emit = restore(it->second);
	
	if (cid==false)
	{
		m_emit_cache0.insert(std::pair<uint64_t, EmitType*>(key, emit));
	}
	else
	{
		m_emit_cache1.insert(std::pair<uint64_t, EmitType*>(key, emit));
	}
}

EmitType* MRInterResult::getEmit(uint64_t key, bool cid)
{
	if (mode!=IR_READING)
	{
		throw hException("MRInterResult::getEmit error mode!=IR_READING");
	}
	if (cid==false)
	{
		std::tr1::unordered_map<uint64_t, EmitType*>::iterator cache_it = m_emit_cache0.find(key);

		if (cache_it != m_emit_cache0.end())
		{
			EmitType *emit = cache_it->second;
			m_emit_cache0.erase(cache_it);
			return emit;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		std::tr1::unordered_map<uint64_t, EmitType*>::iterator cache_it = m_emit_cache1.find(key);

		if (cache_it != m_emit_cache1.end())
		{
			EmitType *emit = cache_it->second;
			m_emit_cache1.erase(cache_it);
			return emit;
		}
		else
		{
			return NULL;
		}
	}
}

void MRInterResult::condWaitCache(bool cid)
{
	if (!cid)
	{
		m_cache0_ready_lock.wait();
	}
	else if (cid)
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
	Int64VecPtr keys( new std::vector<uint64_t> );
	FileMap::iterator filemap_it = m_file_map->begin();
	FileMap::iterator filemap_end = m_file_map->end();
	
	while (filemap_it != filemap_end)
	{
		keys->push_back(filemap_it->first);
		filemap_it++;
	}
	return keys;
}

std::string MRInterResult::getFileName()
{
	return m_filename;
}

size_t MRInterResult::size()
{
	return m_size;
}

