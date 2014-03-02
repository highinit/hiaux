#include "string_utils.h"
#include <sstream>
#include <iostream>
#include <curl/curl.h>
#include <set>

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::vector<std::string> &split(const std::string &s,
							const std::vector<char> &delims,
							std::vector<std::string> &elems)
{
	//std::cout << "PARSING: \n" << s << std::endl;
	
	std::vector<std::string> in_tokens;
	in_tokens.push_back(s);
	
	for (size_t i = 0; i<delims.size(); i++) {
		std::vector<std::string> out_tokens;
		for (size_t j = 0; j<in_tokens.size(); j++)
			if (in_tokens[j]!="")
				split(in_tokens[j], delims[i], out_tokens);
		in_tokens = out_tokens;
	}
	for (int i = 0; i<in_tokens.size(); i++)
		if (in_tokens[i]!="")
			elems.push_back(in_tokens[i]);
	return elems;
}

void fix_utf8_string(std::string& str)
{
	try {
		std::string temp;
		utf8::replace_invalid(str.begin(), str.end(), back_inserter(temp));
		str = temp;
	} catch (utf8::not_enough_room ex) {
	
	}
}

void splitUtf8(const std::string &_s, std::set<uint32_t> &_delims,
				std::vector<std::string> &_elems)
{
	if (_s.size()==0)
		return;
	
	const char *it = _s.data();
	const char *end = _s.data()+_s.size();
	char *elem = new char [_s.size()];
	char *elem_end = elem;
	
	memset(elem, 0, _s.size());
	do {
		uint32_t symbol = utf8::next(it, end);
		
		if (_delims.find(symbol) != _delims.end()) {
			if (elem != elem_end)
				_elems.push_back(std::string(elem));
			memset(elem, 0, _s.size());
			elem_end = elem;
		} else
			elem_end = utf8::append(symbol, elem_end);
			
	} while (it < end);
	
	if (elem != elem_end)
		_elems.push_back(std::string(elem));
	
	delete [] elem;
}

void splitIntoTwoUtf8(const std::string &_v, uint32_t _delim,
						std::string &_f, std::string &_s)
{
	if (_v.size()==0)
		return;
	const char *it = _v.data();
	const char *end = _v.data()+_v.size();
	char *f = new char [_v.size()+1];
	char *f_end = f;
	char *s = new char [_v.size()+1];
	char *s_end = s;
	
	memset(f, 0, _v.size()+1);
	memset(s, 0, _v.size()+1);
	bool first = true;
	
	try {
		do {
			uint32_t symbol = utf8::next(it, end);
			if (first && symbol == _delim) {
				first = false;
				continue;
			}

			if (first)
				f_end = utf8::append(symbol, f_end);
			else
				s_end = utf8::append(symbol, s_end);

		} while (it < end);
	} catch (...)  {
	}
	
	_f = std::string(f);
	_s = std::string(s);
	
	delete [] f;
	delete [] s;
}

bool isUtf8Char(uint32_t c)
{
	// A to Z
	if (0x41 <= c && c <= 0x5a) return true;
	// a to z
	if (0x61 <= c && c <= 0x7a) return true;
	// ?? to ??
	if (0x410 <= c && c <= 0x42F) return true;
	// ?? to ??
	if (0x430 <= c && c <= 0x44f) return true;
	return false;
}

void eraseNonCharsUtf8(std::string &_s)
{
	if (_s.size()==0)
		return;
	
	const char *it = _s.data();
	const char *end = _s.data()+_s.size();
	char res[_s.size()+1];
	char *res_end = res;
	
	memset(res, 0, _s.size()+1);
	
	try {
		do {
			uint32_t symbol = utf8::next(it, end);

			if (isUtf8Char(symbol)) {
				res_end = utf8::append(symbol, res_end);
			} else
				res_end = utf8::append(0x20, res_end); // ' '

		} while (it < end);
	}
	catch (...) {
		
	}
	res_end = 0;
	_s = std::string(res);
}

uint32_t toLowerCharUtf8(uint32_t c)
{
	if (0x41 <= c && c <= 0x5a)
		return c + 0x61 - 0x41;
	if (0x410 <= c && c <= 0x42F)
		return c + 0x430 - 0x410;
	return c;
}

void toLowerUtf8(std::string &_s)
{
	if (_s.size()==0)
	return;
	
	const char *it = _s.data();
	const char *end = _s.data()+_s.size();
	char res[_s.size()+1];
	char *res_end = res;
	
	memset(res, 0, _s.size()+1);
	
	try {
		do {
			uint32_t symbol = utf8::next(it, end);
			res_end = utf8::append(toLowerCharUtf8(symbol), res_end);

		} while (it < end);
	}
	catch (...) {	
	}
	res_end = 0;
	_s = std::string(res);
}

void removeLeadingSpaces(std::string &s)
{
	size_t last_leading_space_pos;
	if (s[0]!=' ')
		return;
	last_leading_space_pos = 0;
	while (s[last_leading_space_pos]==' ' && last_leading_space_pos<s.size())
		last_leading_space_pos++;
	s = s.substr(last_leading_space_pos, s.size()-last_leading_space_pos);
}

void removeEndingSpaces(std::string &s)
{
	size_t last_leading_space_pos;
	if (s[s.size()-1]!=' ')
		return;
	last_leading_space_pos = s.size()-1;
	while (s[last_leading_space_pos]==' ' && last_leading_space_pos>0)
		last_leading_space_pos--;
	s = s.substr(0, last_leading_space_pos);
}

void removeLeadingAndEndingSpaces(std::string &s)
{
	removeLeadingSpaces(s);
	removeEndingSpaces(s);
}

bool getPairGET(const std::string &s, std::pair<std::string, std::string> &kv)
{
	size_t eq_sym_pos = s.find('=');
	if (eq_sym_pos == std::string::npos)
		return false;
	std::string key = s.substr(0, eq_sym_pos);
	std::string value = s.substr(eq_sym_pos+1, s.size()-eq_sym_pos);
	removeLeadingAndEndingSpaces(key);
	removeLeadingAndEndingSpaces(value);
	char *key_c = curl_easy_unescape(NULL, key.c_str(), key.size(), NULL);
	if (key_c==NULL) return false;
	kv.first = std::string(key_c);
	curl_free(key_c);
	
	char *value_c = curl_easy_unescape(NULL, value.c_str(), value.size(), NULL);
	if (value_c==NULL) return false;
	kv.second = std::string(value_c);
	curl_free(value_c);
	
	return true;
}
	
void parseGET(const std::string &_data,
			hiaux::hashtable<std::string, std::string> &values_GET)
{
	std::cout << "parseGET " << _data << std::endl;
	std::vector<std::string> words = split(_data, ' ');
	if (words.size()<2)
		return;
	
	std::string data = words[1];
	
	if (data.size()<4) return;
	std::vector<std::string> keyvalues;
	/*
	std::set<uint32_t> delims;
	delims.insert(0x3f); // '?'
	delims.insert(0x26); // '&'
	
	splitUtf8(data, delims, keyvalues);
	*/
	
	std::vector<char> delims;
	delims.push_back('?');
	delims.push_back('&');
	split(data, delims, keyvalues);
	
	for (int i = 0; i<keyvalues.size(); i++) {
		std::pair<std::string, std::string> kv;
		if (getPairGET(keyvalues[i], kv))
			//std::cout << "KEY: " << kv.first << " VALUE: " << kv.second << std::endl;
			values_GET[kv.first] = kv.second;
	}
}

inline bool isDelimeter(char c)
{
	//if (c==' ') return true;
	if (c=='\t') return true;
	if (c=='\n') return true;
	return false;
}

void removeRedundantDelimeters(std::string &s)
{
//	std::wstring();
	bool prev_delim = false;
	std::string::iterator it = s.begin();
	while (it != s.end()) {
		if (isDelimeter(*it)) {
			if (prev_delim) {
				s.erase(it);
				//prev_delim = false;
			}
			else
				prev_delim = true;
		} else {
			prev_delim = false;
		}
		it++;
	}
}

void string_to_uint64(const std::string &_s, uint64_t &_i)
{
	sscanf(_s.c_str(), "%llu", &_i);
}

uint64_t string_to_uint64(const std::string &_s)
{
	uint64_t i;
	string_to_uint64(_s, i);
	return i;
}

void uint64_to_string(const uint64_t &_i, std::string &_s)
{
	char bf[50];
	sprintf(bf, "%llu", _i);
	_s = std::string(bf);
}

std::string uint64_to_string(const uint64_t &_i)
{
	char bf[50];
	sprintf(bf, "%llu", _i);
	return std::string(bf);
}

std::string float_to_string(float _f)
{
	char bf[50];
	sprintf(bf, "%f", _f);
	return std::string(bf);
}

std::string &unescapeUrl(std::string &_value) {
	char *value_c = curl_easy_unescape(NULL, _value.c_str(), _value.size(), NULL);
	if (value_c==NULL) _value = "";
	_value = std::string(value_c);
	curl_free(value_c);
	return _value;
}

std::string &replaceCharUtf8(std::string &_value, uint32_t _what, uint32_t _to)
{
	if (_value.size()==0)
	return _value;
	
	const char *it = _value.data();
	const char *end = _value.data()+_value.size();
	char *res = new char [_value.size()+1];
	char *res_end = res;
	
	memset(res, 0, _value.size()+1);
	try {
		do {
			uint32_t symbol = utf8::next(it, end);
			if (symbol == _what)
				res_end = utf8::append(_to, res_end);
			else
				res_end = utf8::append(symbol, res_end);

		} while (it < end);
	}
	catch (...) {	
	}
	res_end = 0;
	_value = std::string(res);
	delete [] res;
	return _value;
}
