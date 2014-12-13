#ifndef _LOADCONF_H_
#define _LOADCONF_H_

#include "hiconfig.h"
#include "hiaux/structs/hashtable.h"
#include <jansson.h>
#include <vector>
#include <string>

#include <iostream> 
#include <sstream>

class LoadConf {

	static void doLoadRequired(json_t *root, const std::vector<std::string> &_required, hiaux::hashtable<std::string,std::string> &_params,
								const std::string &_config_file);
	static void doLoadOptional(json_t *root, const std::vector<std::string> &_optional, hiaux::hashtable<std::string,std::string> &_params);
	
public:
	static hiaux::hashtable<std::string,std::string> load (const std::string &_config_file,
														const std::vector<std::string> &_required);
														
	static hiaux::hashtable<std::string,std::string> load (const std::string &_config_file,
														const std::vector<std::string> &_required,
														const std::vector<std::string> &_optional);

};

#endif // _LOADCONF_H_
