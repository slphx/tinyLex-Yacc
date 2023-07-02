#pragma once
#include "common.h"

using namespace std;

class NFANode;
class NFAEdge;
class NFA;

typedef shared_ptr<NFANode> NFANodePtr;
typedef shared_ptr<NFAEdge> NFAEdgePtr;
typedef shared_ptr<NFA> NFAPtr;

class NFANode {
public:
	int id;
	string state;
	NFANode(int i, string s = "") : id(i), state(s) {}
};

class NFAEdge {
public:
	unordered_map<char, set<int>> trans;
	set<int> epsTrans;
	NFAEdge() {}
	void addTrans(char ch, int i) {
		trans[ch].insert(i);
	}
	void addTrans(int i) {
		epsTrans.insert(i);
	}

	void print() {
		for (auto c : trans) {
			for (auto t : c.second) {
				std::cout << '\t' << c.first << "  ->  " << t << '\n';
			}
		}
		for (auto c : epsTrans) {
			std::cout << '\t' << "eps" << "  ->  " << c << '\n';
		}
	}
};

class NFA {
public:
	NFA();

	vector<NFANodePtr> nodes;
	vector<NFAEdgePtr> edges;

	// 入栈优先级和出栈优先级，使用.作为连接符
	static unordered_map<char, int> priorityin;
	static unordered_map<char, int> priorityout;

	int idCount;

	// 判断是否为操作符
	static bool isop(char ch);

	// 单个正则表达式转NFA
	void formRegular(string re, string state = "");

	// 比较算符优先级，若有符号出栈，则进行算符运算
	void check(stack<char>& op, stack<pair<int, int>>& opn, char o);

	// 求闭包
	void closure(set<int>& s);

	void print();
};