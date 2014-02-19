/* 
 * File:   hashtable.h
 * Author: phrk
 *
 * Created on February 18, 2014, 12:15 PM
 */

#ifndef HASHTABLE_H
#define	HASHTABLE_H

#ifdef NO_STD_TR1
	#include <unordered_map>
	namespace hiaux {	
		template<class K, class V>
		class hashtable : public ::std::unordered_map<K,V> {
		};
	}
#else
	#include <tr1/unordered_map>
	
	namespace hiaux {
		template<class K, class V>
		class hashtable : public ::std::tr1::unordered_map<K,V> {
		};
	}
#endif


#endif	/* HASHTABLE_H */

