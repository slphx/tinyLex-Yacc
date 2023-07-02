#pragma once
#include "Common.h"

using namespace std;

class Rule;

typedef shared_ptr<Rule> rulePtr;

// 记录文法
class Rule {
public:
	Rule(symbol s) :origin(s) {}

	int origin;
	vector<vector<int>> expansions;

	// 判断这条规则是否为ε规则
	bool isEps() const;

	// 产生式后附加符号
	void append(int i, int s);
	// 添加产生式 返回新增产生式索引
	int addExpansion();

	void print();
	void print(int index, int dot);
};
