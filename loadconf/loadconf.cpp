#include "loadconf.h"

hiaux::hashtable<std::string,std::string> LoadConf::load (const std::string &_config_file, const std::vector<std::string> &_required) {
	
	hiaux::hashtable<std::string,std::string> ret;
	FILE *f = fopen(_config_file.c_str(), "r");
	json_error_t error;
	json_t *root = json_loadf(f, 0, &error);
	fclose(f);

	if (root == NULL)
		throw std::string("LoadConf::load could not parse file ") + _config_file;

	for (int i = 0; i<_required.size(); i++) {

		json_t *j_param = json_object_get(root, _required[i].c_str());
		if (!json_is_string(j_param))
			throw std::string("LoadConf::load ") + _config_file + " : " + _required[i] + " not set";
		
		ret[ _required[i] ] = json_string_value(j_param);
	}
	return ret;
}