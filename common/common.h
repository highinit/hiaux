
#ifndef _HIAUX_COMMON_H_
#define _HIAUX_COMMON_H_

#include <time.h>
#include <stdint.h>

#include <boost/shared_ptr.hpp>
#include <memory>

class Thing {
public:
	virtual ~Thing();
};

typedef boost::shared_ptr<Thing> ThingPtr;

typedef uint64_t Ts;

Ts getMinuteBeginning();
Ts getHourBeginning();


#endif
