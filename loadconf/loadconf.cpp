#include "loadconf.h"

void LoadConf::doLoadRequired(const Json::Value &_root, const std::vector<std::string> &_required, ConfigParams &_params, 
								const std::string &_config_file) {
	
	for (int i = 0; i<_required.size(); i++) {

		if (_root[ _required[i] ].type() != Json::stringValue)
			throw std::string("LoadConf::load ") + _config_file + " : " + _required[i] + " not set";

		_params[ _required[i] ] = _root[ _required[i] ].asString();
	}
}

void LoadConf::doLoadOptional(const Json::Value &_root, const std::vector<std::string> &_optional, ConfigParams &_params) {
	
	for (int i = 0; i<_optional.size(); i++)
		_params[ _optional[i] ] = _root[ _optional[i] ].asString();
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

void LoadConf::parseFile(const std::string &_filename, Json::Value &_root) {
	
	std::string content;
	
	try {
	
		getFileContents(_filename, content);
		replaceEnvParams(content);
		
	} catch (std::string e) {
		
		throw std::string("LoadConf::load could not parse file ") + _filename + " error: " + e;
		
	} catch (...) {
		throw std::string("LoadConf::load could not open file ") + _filename;
	}
	
	Json::Reader reader;
	if (!reader.parse(content, _root, false))
		throw std::string("LoadConf::load could not parse file ") + _filename;
}

ConfigParams LoadConf::load (const std::string &_config_file, const std::vector<std::string> &_required) {
	
	ConfigParams ret;
	Json::Value root;
	
	parseFile(_config_file, root);	
	doLoadRequired(root, _required, ret, _config_file);
	
	return ret;
}

ConfigParams LoadConf::load (const std::string &_config_file,
														const std::vector<std::string> &_required,
														const std::vector<std::string> &_optional) {

	ConfigParams ret;
	Json::Value root;
	
	parseFile(_config_file, root);	
	doLoadRequired(root, _required, ret, _config_file);
	doLoadOptional(root, _optional, ret);
	
	return ret;
}
