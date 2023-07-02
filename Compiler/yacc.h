#pragma once
#include "common.h"
#include "rule.h"
#include "LALRParser.h"

class Yacc {
public:
	string inFileName;
	string content;
	
	GrammarPtr grammarP;
	LALRParserPtr parser;

	Yacc(string s);

	// 解析语法文件
	void parseGrammar();

	// 辅助方法
	bool isIgnore(char ch) {
		return ch == ' ' || ch == '\n' || ch == '\t';
	}
	bool isCanBeSkip(char ch) {
		return ch == ' ' || ch == '\t';
	}
};

typedef shared_ptr<Yacc> YaccPtr;