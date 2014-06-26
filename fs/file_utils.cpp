#include "file_utils.h"

void listFiles(const std::string &_path, std::vector<std::string> &_files) {

	_files.clear();
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (_path.c_str())) != NULL) {
		
		while ((ent = readdir (dir)) != NULL) {
	    _files.push_back(ent->d_name);
	  }
	  closedir (dir);
	} else {
		throw "listFiles: Could not open dir\n";
	}
}
