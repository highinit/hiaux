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

#endif	/* STRING_UTILS_H */

