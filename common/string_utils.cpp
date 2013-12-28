#include "string_utils.h"
#include <sstream>
#include <iostream>

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
