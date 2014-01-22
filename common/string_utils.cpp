#include "string_utils.h"
#include <sstream>
#include <iostream>
#include <curl/curl.h>

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
			std::tr1::unordered_map<std::string, std::string> &values_GET)
{
	std::vector<std::string> words = split(_data, ' ');
	if (words.size()<2)
		return;
	
	std::string data = words[1];
	
	if (data.size()<4) return;
	std::vector<char> delims;
	delims.push_back('?');
	delims.push_back('&');

	std::vector<std::string> keyvalues;
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
	if (c==' ') return true;
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
			if (prev_delim)
				s.erase(it);
			else
				prev_delim = true;
		} else {
			prev_delim = false;
		}
		it++;
	}
}