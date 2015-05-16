#ifndef _HIAUX_CT_H_
#define _HIAUX_CT_H_

#include <map>
#include <set>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "CtObj.h"
#include "CtEx.h"

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
