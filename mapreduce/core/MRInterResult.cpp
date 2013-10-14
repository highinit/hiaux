#include "MRInterResult.h" 

#include <sys/types.h>
#include <dirent.h>

#include <sys/uio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/uio.h> 
#include <unistd.h>

MRInterResult::MRInterResult(int fd, EmitDumper* dumper):
	m_fd(fd),
	m_dumper(dumper),
	m_cache0_ready_lock(boost::bind(&MRInterResult::checkCacheReady, this, m_cache0_ready)),
	m_cache1_ready_lock(boost::bind(&MRInterResult::checkCacheReady, this, m_cache1_ready))
{

}

bool MRInterResult::checkCacheReady(bool cid)
{
	if (cid==0)
	{
		return m_cache0_ready;
	}
	else
	{
		return m_cache1_ready;
	}
}

void MRInterResult::addEmit(int64_t key, EmitType *emit)
{
	std::string dump = m_dumper->dump(emit);
	delete emit;

	off_t offset = lseek(m_fd, 0, SEEK_END);
	size_t size = dump.size();

	iovec atom[3];
	atom[0].iov_base =  &key;
	atom[0].iov_len = sizeof(int64_t);

	atom[1].iov_base =  &size;
	atom[1].iov_len = sizeof(size_t);

	atom[2].iov_base = (void*)dump.data();
	atom[2].iov_len = size;

	writev(m_fd, atom, 3);
	dump.clear();
	
	m_file_map.insert(std::pair<int64_t, off_t>(key, offset));
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
		std::cout << "no key in filemap\n";
		exit(0);
		return;
		//throw "MRInterResult::preload: No such key";
	}
	
	EmitType *emit = restore(it->second);
	
	if (cid==0)
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
	if (cid==0)
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
			throw "MRInterResult::getEmit Not in cache";
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
			throw "MRInterResult::getEmit Not in cache";
		}
	}
}

void MRInterResult::condWaitCache(bool cid)
{
	if (cid==0)
	{
		m_cache0_ready_lock.wait();
	}
	else
	{
		m_cache1_ready_lock.wait();
	}
}

void MRInterResult::clearCache(bool cid)
{
	if (cid==0)
	{
		m_cache0_ready = 0;
	}
	else
	{
		m_cache1_ready = 0;
	}
}

void MRInterResult::setCacheReady(bool cid)
{
	if (cid==0)
	{
		m_cache0_ready = 1;
		m_cache0_ready_lock.kick();
	}
	else
	{
		m_cache1_ready = 1;
		m_cache1_ready_lock.kick();
	}
}

std::vector<int64_t> MRInterResult::getKeys()
{
	std::vector<int64_t> keys;
	auto filemap_it = m_file_map.begin();
	auto filemap_end = m_file_map.end();
	
	while (filemap_it != filemap_end)
	{
		keys.push_back(filemap_it->first);
		filemap_it++;
	}
	return keys;
}
