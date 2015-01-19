#include "ct.h"

bool CtObjCmp::operator() (CtObjPtr _a, CtObjPtr _b) {
	
	return _a->id < _b->id;
}

json_t* CtObj::toJsonObj() {
	
	json_t *root = json_object();
	
	json_object_set(root, "id", json_integer(id));
	
	json_t *jtypes = json_array();
	
	{
		auto it = types.begin();
		auto end = types.end();
	
		while (it != end) {
			json_array_append_new(jtypes, json_integer(*it));
			it++;
		}
	}
	
	json_t *jfields = json_array();
	{
		auto it = fields.begin();
		auto end = fields.end();
		
		while (it != end) {
			
			json_t* field = json_object();
			json_object_set(root, "k", json_string(it->first.c_str()));
			json_object_set(root, "v", json_string(it->second.c_str()));
			
			json_array_append_new(jfields, field);
			it++;
		}
	}
	
	json_object_set(root, "types", jtypes);
	json_object_set(root, "fields", jfields);
	
	return root;
}


CtTypes::CtTypes(const std::string &_ct_file) {
	
	throw "not implemented";
}
