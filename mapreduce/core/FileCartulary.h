#ifndef READFILECARTUALRY_H
#define READFILECARTUALRY_H

#include <tr1/unordered_map>
#include <string>

#include <boost/function.hpp>

class ReadFileRent
{
	std::tr1::unordered_map<int, int> filecache;
	boost::function<std::string(int id)> m_filename_from_id;
public:
	int getReadFile(int fileid);
	ReadFileRent(boost::function<std::string(int id)> filename_from_id);
	~ReadFileRent();
};

class AppendFileDeposit
{
	std::tr1::unordered_map<int, int> filecache;
	boost::function<std::string(int)> m_filename_from_id;
public:
	
	int getAppendFile(int owner);
	
	void close();
	AppendFileDeposit(boost::function<std::string(int id)> filename_from_id);
	~AppendFileDeposit();
};

#endif
