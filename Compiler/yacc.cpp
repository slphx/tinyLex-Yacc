#include "yacc.h"

Yacc::Yacc(string s) {
	inFileName = s;
	grammarP = make_shared<Grammar>(gSymbolTable.startMark);

	parseGrammar();

	grammarP->calculate();

	parser = make_shared<LALRParser>(LALRParser(grammarP));

}

void Yacc::parseGrammar() {
	ifstream fin(inFileName, fstream::in);
	content.assign(istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
	
	int now = 0, k;
	while (now < content.size()) {
		string var = "", val = "";
		// 跳过一条定义前面的无效字符
		while (isIgnore(content[now])) now++;

		// 获得产生式左边符号
		k = now;
		while (!isIgnore(content[k])) k++;
		var = content.substr(now, k - now);
		now = k;

		// 判断是否为新非终结符，是则加入符号表，返回符号表对应索引
		int origin = gSymbolTable.addSymbol(var, false);

		// 初始状态下 添加增广文法
		if (grammarP->rules.size() == 0) {
			rulePtr r0 = make_shared<Rule>(gSymbolTable.startMark);
			r0->expansions.push_back({});
			r0->expansions[0].push_back(origin);
			grammarP->addRule(r0);
			grammarP->ruleIndex(gSymbolTable.startMark, 0);
		}

		rulePtr ruleP;

		// rule Index 产生式索引
		int ri;

		// 判断是否为新产生式左端
		// 已存在
		if (grammarP->ruleIndex(origin) != -1) {
			ruleP = grammarP->rules[grammarP->ruleIndex(origin)];
			ri = ruleP->addExpansion();
		}
		// 未存在
		else {
			ruleP = make_shared<Rule>(Rule(origin));
			ri = ruleP->addExpansion();
			grammarP->addRule(ruleP);
			grammarP->ruleIndex(origin, grammarP->rules.size() - 1);
		}

		while (isCanBeSkip(content[now])) now++;
		assert(content[now] == ':');
		now++;

		// 获得产生式右边符号
		while (content[now] != ';') {
			while (content[now] != '\n') {
				// 如果ri超出expansions范围 说明是同一origin下的新产生式
				if (ri == ruleP->expansions.size()) ruleP->expansions.push_back({});

				while (isCanBeSkip(content[now])) now++;
				k = now;
				while (!isIgnore(content[k])) k++;
				val = content.substr(now, k - now);
				now = k;

				// 尝试加入gSymbolTable 获得索引
				int i = gSymbolTable.addSymbol(val, false);
				// 加到产生式末尾
				ruleP->expansions[ri].push_back(i);

			}
			now++;
			ri++;
			while (isCanBeSkip(content[now])) now++;
			if (content[now] == '|') now++;
		}
		now++;
		while (isIgnore(content[now])) now++;
	}
}