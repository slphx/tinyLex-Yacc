#pragma once
#include "rule.h"

using namespace std;

class Grammar;

typedef shared_ptr<Grammar> GrammarPtr;
typedef shared_ptr<set<symbol>> setPtr;

class Grammar {
public:
	Grammar(symbol s);

	int start;							// 文法开始符号
	unordered_map<symbol, setPtr> first;		// 记录first集合
	unordered_map<symbol, setPtr> follow;		// 记录follow集合
	setPtr allNSymbols;			// 记录所有非终结符
	setPtr nullSet;			// 记录可以为空的非终结符的集合
	vector<rulePtr> rules;				// 记录产生式

	unordered_map<int, int> ruleIndexMap;	// 左符号到产生式的索引，便于查找产生式

	// 添加产生式
	void addRule(const rulePtr& r);

	// 计算First、Follow集合
	void calculate();

	// 判断符号s当前是否可以为空
	bool ifEps(symbol s);

	// 将s插入t 若发生改变则返回true
	static bool updateSet(setPtr t, setPtr s);
	static bool updateSet(setPtr t, symbol s);

	void ruleIndex(int i, int j);
	int ruleIndex(int i);

	void printSets();
	void printRules();
};