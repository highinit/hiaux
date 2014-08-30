#ifndef _AHO_CORASICK_H_
#define _AHO_CORASICK_H_

#include "hiconfig.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <queue>

class BorNode {
private:
	
	
	static int m_nnodes;
public:
	
	int id;
	
	std::vector<std::string> m_matches;
	std::map<char, BorNode*> m_children;
	BorNode *fail_node;
	
	BorNode(const std::vector<std::string> &_parent_matches);
	void addWord(const std::string &_word, const std::string &_suff);
	BorNode *getFallNode(char _c);
	void setFailNode(BorNode *_node);
	void pushChildrenToQueue(std::queue<BorNode*> &_ch);
	
	void print(size_t _ntabs);
};

class AhoCorasick {

	std::vector<std::string> m_dict;
	BorNode *m_root;
public:
	
	AhoCorasick(const std::vector<std::string> &_dict);
	void findMatches(const std::string &_text, std::vector< std::pair<std::string, size_t> > &_matches);
	void print();
};

#endif
