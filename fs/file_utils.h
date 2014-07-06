#ifndef FILE_UTILS_H
#define	FILE_UTILS_H

#include "hiconfig.h"

#include <string>
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void listFiles(const std::string &_path, std::vector<std::string> &_files);

#endif
