#include "AhoCorasick.h"

int BorNode::m_nnodes = -1;

BorNode::BorNode():
	 fail_node(NULL),
	 id(++BorNode::m_nnodes) {
	
}

void BorNode::addWord(const std::string &_word, const std::string &_suff) {
	
	if (_suff.size() == 0) {
		
		m_matches.push_back(_word);
		return;
	}
	
	std::map<char, BorNode*>::iterator it = m_children.find(_suff[0]);
	if (it == m_children.end())
		m_children.insert(std::pair<char, BorNode*>(_suff[0], new BorNode()));
	
	m_children[ _suff[0] ]->addWord( _word, _suff.substr(1) );
}

void BorNode::setFailNode(BorNode *_node) {
	fail_node = _node;
}

void BorNode::pushChildrenToQueue(std::queue<BorNode*> &_ch) {
	
	std::map<char, BorNode*>::iterator it = m_children.begin();
	std::map<char, BorNode*>::iterator end = m_children.end();
	while (it != end) {
		
		_ch.push(it->second);
		it++;
	}
}

BorNode* BorNode::getFallNode(char _c) {
	
	std::map<char, BorNode*>::iterator it = fail_node->m_children.find(_c);
	if (it != fail_node->m_children.end() ) {
		return it->second;
	} else {
		
		if (this != fail_node)
			return fail_node->getFallNode(_c);
		else return this;
	}
}

void BorNode::print(size_t _ntabs) {
	
	std::string tabs;
	for (int i = 0; i<_ntabs; i++)
		tabs += "\t"; 
	
	{
		std::map<char, BorNode*>::iterator it = m_children.begin();
		std::map<char, BorNode*>::iterator end = m_children.end();
	
		std::cout << tabs << "id: " << id << std::endl;
		std::cout << tabs << "children: \n";
	
		while (it != end) {
			std::cout << tabs << it->first << std::endl;
			it->second->print(_ntabs + 1);
			it++;
		}
	
		std::cout << std::endl;
	}
	{
		std::cout << tabs << "matches: \n";
		std::cout << tabs;
		for (int i = 0; i<m_matches.size(); i++) {
			std::cout << m_matches[i] << ", ";
		}
		std::cout << std::endl;
	}
	
	std::cout << tabs << "fallnode: " << fail_node->id << std::endl;
	
	std::cout << tabs << "___________" << std::endl;
}

AhoCorasick::AhoCorasick(const std::vector<std::string> &_dict) {

	m_dict = _dict;
	m_root = new BorNode();
	m_root->setFailNode(m_root);

	for (int i = 0; i<m_dict.size(); i++)
		m_root->addWord(m_dict[i], m_dict[i]);
	
	std::queue<BorNode*> fall_q;
	m_root->pushChildrenToQueue(fall_q);
	
	{
		std::map<char, BorNode*>::iterator it = m_root->m_children.begin();
		std::map<char, BorNode*>::iterator end = m_root->m_children.end();
	
		while (it != end) {
			it->second->setFailNode(m_root);
			it++;
		}
	}
	
	while (!fall_q.empty()) {
		BorNode *cur_node = fall_q.front();
		
		
		std::map<char, BorNode*>::iterator it = cur_node->m_children.begin();
		std::map<char, BorNode*>::iterator end = cur_node->m_children.end();
	
		while (it != end) {
			BorNode *fall_node = cur_node->getFallNode( it->first );
			
			it->second->setFailNode( fall_node );
			
			for (int i = 0; i<fall_node->m_matches.size(); i++) {
				it->second->m_matches.push_back(fall_node->m_matches[i]);
			}		
			it++;
		}
		
		cur_node->pushChildrenToQueue(fall_q);
		fall_q.pop();
	}
}

void AhoCorasick::findMatches(const std::string &_text, std::vector< std::pair<std::string, size_t> > &_matches) {
	
	BorNode *cur_node = m_root;
	
	size_t i = 0;
	
	while (i<_text.size()) {
		
		std::map<char, BorNode*>::iterator it = cur_node->m_children.find(_text[i]);
		
		if (it != cur_node->m_children.end()) {
			cur_node = it->second;
		} else {
			
			if (cur_node == cur_node->fail_node)
				i++;
			
			cur_node = cur_node->fail_node;
			continue;
		}
	
		for (int j = 0; j<cur_node->m_matches.size(); j++)
			_matches.push_back(std::pair<std::string, size_t>(cur_node->m_matches[j], i - cur_node->m_matches[j].size() + 1 ));
		i++;
	}
	
}

void AhoCorasick::print() {
	m_root->print(0);
}

