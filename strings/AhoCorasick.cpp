#include "AhoCorasick.h"

int BorNode::m_nnodes = -1;

BorNode::BorNode(const std::vector<std::string> &_parent_matches):
	 m_matches(_parent_matches),
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
		m_children.insert(std::pair<char, BorNode*>(_suff[0], new BorNode(m_matches)));
	
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
	std::vector<std::string> root_matches;
	m_root = new BorNode(root_matches);
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
		
		bool inc = false;
		/*
   while (current_state) {
            BorNode *candidate = current_state->getLink(c);
            if (candidate) {
                current_state = candidate;
                return;
            }
            current_state = current_state->fail;
        }
        current_state = &root;
		*/
		
		//bool node_set = false;
		/*
		while (1) {
			
			if (cur_node->m_children.size() == 0) break;
			
			std::map<char, BorNode*>::iterator it = cur_node->m_children.find(_text[i]);
			if (it != cur_node->m_children.end()) {
				cur_node = it->second;
				break;
			} 
			cur_node = cur_node->fail_node;
			
		}
		*/
		
		std::map<char, BorNode*>::iterator it = cur_node->m_children.find(_text[i]);
		if (it != cur_node->m_children.end()) {
			cur_node = it->second;
			//inc = true;
		} else {
			cur_node = cur_node->fail_node;
			
			//it = cur_node->fail_node->m_children.find(_text[i]);
			//cur_node = it->second;
		}
		
		for (int j = 0; j<cur_node->m_matches.size(); j++)
			_matches.push_back(std::pair<std::string, size_t>(cur_node->m_matches[j], i - cur_node->m_matches[j].size() + 1 ));
		
		//if (inc)
			i++;
		//if (cur_node->m_children.size() == 0) {
		//	cur_node = m_root;
		//	continue;
		//}

	}
	
}

void AhoCorasick::print() {
	m_root->print(0);
}

