/* 
 * File:   string_utils.h
 * Author: phrk
 *
 * Created on December 28, 2013, 7:56 PM
 */

#ifndef STRING_UTILS_H
#define	STRING_UTILS_H

#include <string>
#include <vector>
#include <tr1/unordered_map>

std::vector<std::string> &split(const std::string &s,
							char delim,
							std::vector<std::string> &elems);


std::vector<std::string> split(const std::string &s, char delim);

std::vector<std::string> &split(const std::string &s,
							const std::vector<char> &delims,
							std::vector<std::string> &elems);

void removeLeadingSpaces(std::string &s);
void removeEndingSpaces(std::string &s);
void removeLeadingAndEndingSpaces(std::string &s);

bool getPairGET(const std::string &s, std::pair<std::string, std::string> &kv);
void parseGET(const std::string &data,
			std::tr1::unordered_map<std::string, std::string> &values_GET);

inline bool isDelimeter(char c);
void removeRedundantDelimeters(std::string &s);

void string_to_uint64(const std::string &_s, uint64_t &_i);
uint64_t string_to_uint64(const std::string &_s);
void uint64_to_string(const uint64_t &_i, std::string &_s);
std::string uint64_to_string(const uint64_t &_i);

#endif	/* STRING_UTILS_H */
