/* 
 * File:   hashtable.h
 * Author: phrk
 *
 * Created on February 18, 2014, 12:15 PM
 */

#ifndef HASHTABLE_H
#define	HASHTABLE_H

#include "hiconfig.h"

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

#ifdef NO_STD_TR1
	#include <unordered_map>
/*
	template <class T>
	struct ::std::hash<std::pair<T, T> > {
	public:
        size_t operator()(std::pair<T, T> x) const throw() {   
             return ::std::hash<T>()(x.first) ^ ::std::hash<T>()(x.second);
        }
	};
	
	template <class T, class T2>
	struct ::std::hash<std::pair<T, T2> > {
	public:
        size_t operator()(std::pair<T, T2> x) const throw() {   
             return ::std::hash<T>()(x.first) ^ ::std::hash<T2>()(x.second);
        }
	};
	
	template <class T, class T2, class T3>
	struct ::std::hash< ::boost::tuple<T, T2, T3> > {
	public:
        size_t operator()( ::boost::tuple<T, T2, T3> x) const throw() {   
			return ::std::hash<T>()(::boost::get<0>(x)) ^ ::std::hash<T2>()(::boost::get<1>(x)) ^ ::std::hash<T2>()(::boost::get<2>(x));
        }
	};
*/	
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
	/*
	template <>
	struct ::std::tr1::hash<std::pair<uint64_t, uint64_t> > {
	public:
	        size_t operator()(std::pair<uint64_t, uint64_t> x) const throw() {   
	             return ::std::tr1::hash()(x.first) ^ ::std::tr1::hash()(x.second);
	        }
	};*/
	
#endif


#endif	/* HASHTABLE_H */

