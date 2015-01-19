#ifndef _HIAUX_CT_H_
#define _HIAUX_CT_H_

#include <map>
#include <set>
#include <vector>
#include <jansson.h>
#include <boost/shared_ptr.hpp>

typedef uint64_t CtTypeId;
typedef uint64_t CtObjId;

class CtObj {
public:
	
	CtObjId id;
	std::set<CtTypeId> types;
	std::map<std::string, std::string> fields;
	
	json_t *toJsonObj();
};

typedef boost::shared_ptr<CtObj> CtObjPtr;

class CtObjCmp {
public:
	
	bool operator() (CtObjPtr _a, CtObjPtr _b);
};

class CtType {
public:
	
	class Field {
	public:
		std::string name;
		std::string caption;
	//	std::string expl_value;
	//	std::vector<std::string> possible_values;
	};
	
	CtTypeId id;
	CtTypeId pid;
	
	std::string nameru;
	std::string nameen;

	std::map<std::string, Field> fields;
	std::vector<std::string> children;	
};

class CtTypes {
public:
	
	std::map<CtTypeId, CtType> types;
	
	CtTypes(const std::string &_ct_file);
	
};

#endif
