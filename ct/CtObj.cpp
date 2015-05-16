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
	
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(_dump, root, false))		
		throw CtEx("CtObj::parseDump cant parse");
	
	Json::Value jtypes = root["types"];
		
	if (jtypes.type() != Json::arrayValue)
		throw CtEx("CtObj::parseDump root[\"types\"] not array");
	
	for (int i = 0; i<jtypes.size(); i++)
		types.insert( jtypes[i].asInt() );
		
	Json::Value jfields = root["fields"];
	
	auto it = jfields.begin(); 
	auto end = jfields.end();
	
	while (it != end) {
		
		auto v = jfields [ it.key().asString() ];
		if (v.type() == Json::intValue || v.type() == Json::uintValue || v.type() == Json::realValue) {
			
			fields [ it.key().asString() ] = inttostr(v.asInt());
			
		} else if (v.type() == Json::stringValue) {
			
			fields [ it.key().asString() ] = v.asString();
			
		} else {
			
			throw CtEx("CtObj::parseDump wrong field");
		}
		
		it++;
	}
}

bool CtObj::inType(CtTypeId _typeid) {
	
	return types.find(_typeid) != types.end();
}
