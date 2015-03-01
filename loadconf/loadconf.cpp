#include "loadconf.h"

void LoadConf::doLoadRequired(json_t *root, const std::vector<std::string> &_required, ConfigParams &_params, 
								const std::string &_config_file) {
	
	for (int i = 0; i<_required.size(); i++) {

		json_t *j_param = json_object_get(root, _required[i].c_str());
		if (!json_is_string(j_param)) {
			json_decref(root);
			throw std::string("LoadConf::load ") + _config_file + " : " + _required[i] + " not set";
		}
		
		_params[ _required[i] ] = json_string_value(j_param);
	}
}

void LoadConf::doLoadOptional(json_t *root, const std::vector<std::string> &_optional, ConfigParams &_params) {
	
	for (int i = 0; i<_optional.size(); i++) {

		json_t *j_param = json_object_get(root, _optional[i].c_str());
		if (json_is_string(j_param)) {
			_params[ _optional[i] ] = json_string_value(j_param);
		}
	}
}

void LoadConf::replaceEnvParams(std::string &_text) {
	
	std::string ret;
	
	size_t doll_pos = _text.find("$");
	size_t prev_close = 0;
	
	while (doll_pos != std::string::npos) {
		
		size_t close_pos = _text.find("}", doll_pos);
		
		if (close_pos == std::string::npos)
			throw std::string("LoadConf::replaceEnvParams '}' not found");
		
		std::string var = _text.substr(doll_pos+2, close_pos-doll_pos-2);
		
		ret.append( _text.substr(prev_close, doll_pos-prev_close) );
		
		char *value = getenv(var.c_str());
		
		if (value == NULL)
			throw std::string("LoadConf::replaceEnvParams variable ") + var + " not set";
		
		ret.append( value );
		
		prev_close = close_pos + 1;
		
		doll_pos = _text.find("$", prev_close);
	}
	
	ret.append( _text.substr(prev_close, _text.size() - prev_close -1 ) );
	_text = ret;	
}

json_t* LoadConf::parseFile(const std::string &_filename) {
	
	std::string content;
	
	try {
	
		getFileContents(_filename, content);
		replaceEnvParams(content);
		
	} catch (std::string e) {
		
		throw std::string("LoadConf::load could not parse file ") + _filename + " error: " + e;
		
	} catch (...) {
		throw std::string("LoadConf::load could not open file ") + _filename;
	}
	
	json_error_t error;
	json_t *root = json_loads(content.c_str(), 0, &error);
	
	if (root == NULL)
		throw std::string("LoadConf::load could not parse file ") + _filename;
	
	return root;
}

ConfigParams LoadConf::load (const std::string &_config_file, const std::vector<std::string> &_required) {
	
	ConfigParams ret;
	
	json_t* root = parseFile(_config_file);
	doLoadRequired(root, _required, ret, _config_file);
	json_decref(root);
	
	return ret;
}

ConfigParams LoadConf::load (const std::string &_config_file,
														const std::vector<std::string> &_required,
														const std::vector<std::string> &_optional) {

	ConfigParams ret;
	json_t* root = parseFile(_config_file);
	
	doLoadRequired(root, _required, ret, _config_file);
	doLoadOptional(root, _optional, ret);
	
	json_decref(root);
	return ret;
}
