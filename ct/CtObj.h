#ifndef _HIAUX_CT_OBJ_H_
#define _HIAUX_CT_OBJ_H_

#include <map>
#include <set>
#include <vector>
#include <jansson.h>
#include <boost/shared_ptr.hpp>

#include <iostream>

#include "hiaux/strings/string_utils.h"

typedef uint64_t CtTypeId;
typedef uint64_t CtObjId;

CtTypeId CtTypeIdFromString(const std::string &_s);
CtObjId CtObjIdFromString(const std::string &_s);

class CtObj {
	
	void parseDump(const std::string &_dump);
	
public:
	
	CtObjId id;
	int grp;
	std::set<CtTypeId> types;
	std::map<std::string, std::string> fields;
	
	std::string dump;
	
	CtObj(CtTypeId _id, const std::string &_dump);
	CtObj(CtTypeId _id, const std::string &_dump, int _grp);
	bool inType(CtTypeId _typeid);
};

typedef boost::shared_ptr<CtObj> CtObjPtr;

#endif
