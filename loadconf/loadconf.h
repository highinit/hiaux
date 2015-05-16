#ifndef _LOADCONF_H_
#define _LOADCONF_H_

#include "hiconfig.h"
#include <json/json.h>
#include <vector>
#include <map>
#include <string>

#include <iostream> 
#include <sstream>

#include <stdlib.h>

#include "hiaux/strings/string_utils.h"

typedef std::map<std::string,std::string> ConfigParams;

class LoadConf {

	static void doLoadRequired(const Json::Value &_root, const std::vector<std::string> &_required, ConfigParams &_params,
								const std::string &_config_file);
	static void doLoadOptional(const Json::Value &_root, const std::vector<std::string> &_optional, ConfigParams &_params);
	
	static void parseFile(const std::string &_filename, Json::Value &_root);
	static void replaceEnvParams(std::string &_text);
	
public:
	static ConfigParams load (const std::string &_config_file,
														const std::vector<std::string> &_required);
														
	static ConfigParams load (const std::string &_config_file,
														const std::vector<std::string> &_required,
														const std::vector<std::string> &_optional);

};

#endif // _LOADCONF_H_
