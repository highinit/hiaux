#ifndef READFILECARTUALRY_H
#define READFILECARTUALRY_H

#include <unordered_map>
#include <string>

class ReadFileCartulary
{
	std::unordered_map<std::string, int> filecache;
public:
	int getReadFile(std::string filename);
	ReadFileCartulary();
	~ReadFileCartulary();
};

#endif
