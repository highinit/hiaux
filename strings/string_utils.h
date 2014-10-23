/* 
 * File:   string_utils.h
 * Author: phrk
 *
 * Created on December 28, 2013, 7:56 PM
 */

#ifndef STRING_UTILS_H
#define	STRING_UTILS_H

#include "hiconfig.h"

#include <errno.h>
#include <string>
#include <vector>
#include <cstring>
#include "hiaux/structs/hashtable.h"
#include <set>
#include <map>
#include "../../thirdparty/utf8_v2_3_4/source/utf8.h"

// gurantees order
std::vector<std::string> &split(const std::string &s,
							char delim,
							std::vector<std::string> &elems);


std::vector<std::string> split(const std::string &s, char delim);

std::vector<std::string> &split(const std::string &s,
							const std::vector<char> &delims,
							std::vector<std::string> &elems);

void fix_utf8_string(std::string& str);

void splitUtf8(const std::string &_s, std::set<uint32_t> &_delims,
					std::vector<std::string> &_elems);
void splitIntoTwoUtf8(const std::string &_v, uint32_t delim,
						std::string &_f, std::string &_s);

bool isUtf8Char(uint32_t c);
void eraseNonCharsUtf8(std::string &_s);
uint32_t toLowerCharUtf8(uint32_t c);
void toLowerUtf8(std::string &_s);

void removeLeadingSpaces(std::string &s);
void removeEndingSpaces(std::string &s);
void removeLeadingAndEndingSpaces(std::string &s);

bool getPairGET(const std::string &s, std::pair<std::string, std::string> &kv);
void parseGET(const std::string &data,
			hiaux::hashtable<std::string, std::string> &values_GET);

inline bool isDelimeter(char c);
void removeRedundantDelimeters(std::string &s);

void string_to_uint64(const std::string &_s, uint64_t &_i);
uint64_t string_to_uint64(const std::string &_s);
void uint64_to_string(const uint64_t &_i, std::string &_s);
std::string uint64_to_string(const uint64_t &_i);
std::string float_to_string(float _f);
std::string inttostr (int _i);
int strtoint(const std::string &_s);

std::string &escapeUrl(std::string &_url);
std::string &unescapeUrl(std::string &_value);
std::string &replaceCharUtf8(std::string &_value, uint32_t _what, uint32_t _to);

std::string getUrlPath(const std::string &_url);

void getFileContents(const std::string &_filename, std::string &_contents);

bool isStringInVec(const std::string &_str, std::vector<std::string> &_v);

void parseCookies(const std::string &_header, hiaux::hashtable<std::string, std::string> &_cookies);

void replaceSubstring(std::string &_str, const std::string &_from, const std::string &_to);

std::string booltostr(bool _a);

void getBigrams(const std::string &_q, std::vector<std::string> &_bigrams);

std::string escape_symbol(const std::string &_str, char sym);
std::string escape_quote(const std::string &_str);
std::string add_newline_backslash(const std::string &_str);

void getPrefixesUtf8(const std::string &_word, std::vector<std::string> &_prefixes);

#endif	/* STRING_UTILS_H */
