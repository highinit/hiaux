#ifndef _LOADCONF_H_
#define _LOADCONF_H_

#include "hiconfig.h"
#include "hiaux/structs/hashtable.h"
#include <jansson.h>
#include <vector>
#include <string>

class LoadConf {

public:
	static hiaux::hashtable<std::string,std::string> load (const std::string &_config_file,
														const std::vector<std::string> &_required);

};

#endif // _LOADCONF_H_
