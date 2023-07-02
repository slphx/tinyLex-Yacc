#pragma once
#include "nfa.h"
#include "dfa.h"

using namespace std;

class Lex {
public:
	string inFileName;
	string content;
	unordered_map<string, string> variables;
	vector<pair<string, string>> tokenRegex;
	DFAPtr dfaP;

	string tarText;
	int ptr = 0;

	Lex(string s);

	// 解析词法文件，获得Token的正则表达式，放入tokenRegex中
	void parseLex();

	// 构造词法解析dfa
	void formDFA();

	// 加载目标文本
	void loadTarget(string fname);

	Token nextToken(bool& isend);

	// 辅助方法
	bool isIgnore(char ch) {
		return ch == ' ' || ch == '\n' || ch == '\t';
	}
};

typedef shared_ptr<Lex> LexPtr;