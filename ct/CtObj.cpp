#include "CtObj.h"

CtTypeId CtTypeIdFromString(const std::string &_s) {
	return std::stoi(_s);
}

CtObjId CtObjIdFromString(const std::string &_s) {
	return std::stoi(_s);
}

CtObj::CtObj(CtTypeId _id, const std::string &_dump):
id(_id),
dump(_dump) {
	
	parseDump(_dump);
}

CtObj::CtObj(CtTypeId _id, const std::string &_dump, int _grp):
id(_id),
grp(_grp),
dump(_dump) {
	
	parseDump(_dump);
}

void CtObj::parseDump(const std::string &_dump) {
	
	json_error_t error;
	json_t *root = json_loads(_dump.c_str(), 0, &error);
	
	if (!json_is_object(root)) {
		
		throw 1;
	}
	
	json_t *types_list = json_object_get(root, "types");
	
	if (!json_is_array(types_list)) {
		
		json_decref(root);
		throw 1;
	}
	
	for (int i = 0; i<json_array_size(types_list); i++) {		

		json_t *type = json_array_get(types_list, i);
		types.insert( json_integer_value(type) );
	}
	
	//////////
	
	json_t *fields_values = json_object_get(root, "fields");
	
	if (!json_is_object(fields_values)) {
		
		json_decref(root);
		throw 1;
	}
	
	const char *key;
	json_t *value;
	
	json_object_foreach (fields_values, key, value ) {
		
		if (json_is_integer(value)) {
			
			fields [key] = inttostr(json_integer_value(value));
			
		} else if (json_is_string(value)) {
			
			fields [key] = json_string_value(value);
			
		} else {
			
			std::cout << "CtObj::CtObj wrong field\n";
		}
	}
	
	json_decref(root);
}

bool CtObj::inType(CtTypeId _typeid) {
	
	return types.find(_typeid) != types.end();
}
