
#include "common.h"

Thing::~Thing() {
	
}

Ts getMinuteBeginning() {
	
	return time(0)/60;
}

Ts getHourBeginning() {
	
	return time(0)/3600;
}
