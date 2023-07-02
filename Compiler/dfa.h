#pragma once
#include "nfa.h"
#include "common.h"

using namespace std;

class DFANode;
class DFAEdge;
class DFA;

typedef shared_ptr<DFANode> DFANodePtr;
typedef shared_ptr<DFAEdge> DFAEdgePtr;
typedef shared_ptr<DFA> DFAPtr;
typedef shared_ptr<set<int>> setPtr;

class DFANode {
public:
	int id;
	string state;
	DFANode(int i, string s = "") :id(i), state(s) {}
};

class DFAEdge {
public:
	unordered_map<char, int> trans;

	DFAEdge() {}

	void addTrans(char ch, int i) {
		trans[ch] = i;
	}

	void print() {
		for (auto c : trans) {
			std::cout << '\t' << c.first << "  ->  " << c.second << '\n';
		}
	}
};

class DFA {
public:
	vector<DFANodePtr> nodes;
	vector<DFAEdgePtr> edges;
	int idCount = 1;

	DFA(NFAPtr nfaP, unordered_map<string, int>& prio);

	// 返回 state 以 ch 为出边的状态，若无此边则返回 -1
	int next(int state, char ch);
	
	void print();

};