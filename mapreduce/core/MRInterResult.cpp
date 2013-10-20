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
							const size_t wbuffer_cap):
	m_dumper(dumper),
	m_cache0_ready_lock(boost::bind(&MRInterResult::checkCacheReady, this, 0)),
	m_cache1_ready_lock(boost::bind(&MRInterResult::checkCacheReady, this, 1)),
	no_more_writes(0),
	m_wbuffer_cap(wbuffer_cap),
	flush_finished(0),
	flush_finish_lock(boost::bind(&MRInterResult::FlushFinished, this)),
	mode(IR_WRITING),
	m_filename(filename),
	w_offset(0),
	write_queue(100000)
{
	wbuffer = (void*) new uint8_t [wbuffer_cap];
	m_wbuffer_size = 0;
	
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

/*
bool MRInterResult::checkWriteBufferNotEmpty()
{
//	wbuffer_lock.lock();
	bool notempty = (write_queue.size()==0);
//	wbuffer_lock.lock();
	return notempty;
}*/

bool MRInterResult::FlushFinished()
{
	return flush_finished;
}

void MRInterResult::flush(std::pair<int64_t, std::string> dump)
{
	off_t offset = lseek(m_fd, 0, SEEK_END);
	int64_t size = dump.second.size();

	iovec atom[3];
	atom[0].iov_base =  &dump.first;
	atom[0].iov_len = sizeof(int64_t);

	atom[1].iov_base =  &size;
	atom[1].iov_len = sizeof(int64_t);

	atom[2].iov_base = (void*)dump.second.data();
	atom[2].iov_len = size;

	writev(m_fd, atom, 3);
	dump.second.clear();
	
	m_file_map.insert(std::pair<int64_t, int64_t>(dump.first, offset));
}

void MRInterResult::flush_wbuffer()
{
	if (m_wbuffer_size==0)
		return;
	
	iovec atom[1];
	atom[0].iov_base = wbuffer;
	atom[0].iov_len = m_wbuffer_size;
	writev(m_fd, atom, 1);
	m_wbuffer_size = 0;
}

bool MRInterResult::flushBuffer()
{
	//wbuffer_lock.lock();
	
/*
	while (write_buffer.size()!=0)
	{
		flush(write_buffer.front());
		write_buffer.pop();
	}

*/	
/*	
	if (write_queue.size()!=0)
	{
		//lseek(m_fd, 0, SEEK_END);
		iovec atom[3*write_queue.size()];
		std::vector<std::pair<int64_t, std::string> > dumps(write_queue.size());
		std::vector<int64_t> sizes(write_queue.size());
		
		int atom_i = 0;
		int dump_i = 0;

		while (write_queue.size()!=0)
		{
			//flush(write_buffer.front());
			dumps[dump_i] = write_queue.front();
			write_queue.pop();
			sizes[dump_i] = dumps[dump_i].second.size();

			atom[atom_i].iov_base = &dumps[dump_i].first;
			atom[atom_i].iov_len = sizeof(int64_t);

			atom[atom_i+1].iov_base =  &sizes[dump_i];
			atom[atom_i+1].iov_len = sizeof(int64_t);

			atom[atom_i+2].iov_base = (void*)dumps[dump_i].second.data();
			atom[atom_i+2].iov_len = sizes[dump_i];

			
			m_file_map.insert(std::pair<int64_t, int64_t>(dumps[dump_i].first, w_offset));
			w_offset += sizeof(int64_t)+sizeof(int64_t)+sizes[dump_i];
			atom_i+=3;
			dump_i++;
		}
		//std::cout << dumps.size();
		writev(m_fd, atom, atom_i);
		dumps.clear();
		sizes.clear();
	}
*/

	//if (write_queue.size()!=0)
	{	
		while (!write_queue.empty())//(write_queue.size()!=0)
		{				
			std::pair<int64_t, std::string>* kv;
			if (!write_queue.pop(kv))
			{
				break;
			}
			
			int64_t key = kv->first;
			std::string dump = kv->second;
			int64_t size = dump.size();
			//wbuffer_lock.unlock();
			delete kv;
			
			if (2*sizeof(int64_t)+size > m_wbuffer_cap-m_wbuffer_size)
			{
				flush_wbuffer();
			}
			
			memcpy((void*)( (uint8_t*)wbuffer+m_wbuffer_size), &key, sizeof(int64_t));
			m_wbuffer_size +=  sizeof(int64_t);
			memcpy((void*)( (uint8_t*)wbuffer+m_wbuffer_size), &size, sizeof(int64_t));
			m_wbuffer_size +=  sizeof(int64_t);
			memcpy((void*)( (uint8_t*)wbuffer+m_wbuffer_size), dump.data(), size);
			m_wbuffer_size += size;
			
			m_file_map.insert(std::pair<int64_t, int64_t>(key, w_offset));
			w_offset += 2*sizeof(int64_t)+size;
			//wbuffer_lock.lock();
		}
	}
	
	if (no_more_writes.load())
	{
		flush_wbuffer();
		//wbuffer_lock.unlock();
		std::cout << "no more writes: " << m_filename.c_str() << std::endl;
		mode = IR_READING;
		::close(m_fd);
		m_reader = new InterResultLoader(m_filename, m_dumper);

		flush_finish_lock.lock();
		flush_finished = 1;
		flush_finish_lock.kick();
		flush_finish_lock.unlock();

		return 0; // dont repeat
	}
	//wbuffer_lock.unlock();
	return 1; // repeat
}

void MRInterResult::waitFlushFinished()
{
//	wbuffer_lock.lock();	
	no_more_writes = 1;
//	wbuffer_lock.unlock();
	std::cout << "waiting flush finished\n";
	flush_finish_lock.wait();
	std::cout << "flush finished\n";
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
	if (mode!=IR_WRITING)
	{
		throw "MRInterResult::addEmit error mode!=IR_WRITING";
	}
	std::string dump = m_dumper->dump(emit);
	delete emit;

	//wbuffer_lock.lock();
	write_queue.push(new std::pair<int64_t, std::string>(key, dump));
	//wbuffer_lock.unlock();
}

EmitType *MRInterResult::restore(off_t offset)
{
	lseek(m_fd, offset, SEEK_SET);
	int64_t size;
	int64_t key;
	if (read(m_fd, &key, sizeof(int64_t))!=sizeof(int64_t))
	{
		throw "MRInterResult::preload: READ ERROR\n";
	}
	
	if (read(m_fd, &size, sizeof(int64_t))!=sizeof(int64_t))
	{
		throw "MRInterResult::preload: READ ERROR\n";
	}
	char bf[size+1];
	if (read(m_fd, bf, size)!=size)
	{
		throw "MRInterResult::preload: READ ERROR\n";
	}
	bf[size] = '\0';
	//std::cout << "restored key: " << key << std::endl; 
	EmitType *emit = m_dumper->restore(bf);
	return emit;
}

void MRInterResult::preload(int64_t key, bool cid)
{
	if (mode!=IR_READING)
	{
		throw "MRInterResult::preload error mode!=IR_READING";
	}
	auto it = m_file_map.find(key);
	if (it==m_file_map.end())
	{
		return;
	}
	
	EmitType *emit = m_reader->readEmit(it->second);
	//EmitType *emit = restore(it->second);
	
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
//	throw "123";
	if (mode!=IR_READING)
	{
		throw "MRInterResult::getEmit error mode!=IR_READING";
	}
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
