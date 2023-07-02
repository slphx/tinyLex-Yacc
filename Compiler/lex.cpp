#include "lex.h"

Lex::Lex(string s) {
	inFileName = s;
	parseLex();
	formDFA();
}

void Lex::parseLex() {
	// 读入文件
	ifstream fin(inFileName, fstream::in);
	content.assign(istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
	
	int now = 0, k;

	while (isIgnore(content[now])) now++;

	// 定义部分
	while (content[now] != '%') {
		string var = "", val = "";
		// 跳过一条定义前面的无效字符
		while (isIgnore(content[now])) now++;
		k = now;
		// 获得定义的变量
		while (!isIgnore(content[k])) k++;
		var = content.substr(now, k - now);
		now = k;
		// 获得定义的对应值
		while (isIgnore(content[now])) now++;
		k = now;
		while (content[k] != '\n') {
			if (!isIgnore(content[k])) {
				if (content[k] == '\\' && (content[k + 1] == '{' || content[k + 1] == '}')) {
					val += content[k + 1]; k += 1;
				}
				else {
					if (content[k] == '{') {
						string thisvar = "";
						int t = k + 1;
						while (content[t] != '}') t++;
						thisvar = content.substr(k + 1, t - k - 1);
						assert(variables.find(thisvar) != variables.end());
						val += '(' + variables[thisvar] + ')';
						k = t;
					} 
					else val += content[k];
				}
			}
			k++;
		}
		variables[var] = val;
		now = k + 1;
		// 跳过一条定义后面的无效字符
		while (isIgnore(content[now])) now++;
	}
	while (content[now] == '%') now++;
	// 主体部分
	while (content[now] != '%') {
		string var = "", val = "";
		// 跳过一条正则前面的无效字符
		while (isIgnore(content[now])) now++;
		k = now;
		// 若为定义正则
		if (content[now] == '{') {
			while (content[k] != '}') k++;
			val = content.substr(now + 1, k - now - 1);
			assert(variables.find(val) != variables.end());
			val = variables[val];
			now = k + 1;
		} 
		// 若为普通正则
		else {
			while (!isIgnore(content[k])) k++;
			val = content.substr(now, k - now);
			now = k;
		}

		while (content[now] == ' ' || content[now] == '\t') now++;
		k = now;
		while (content[k] != '\n') k++;
		var = content.substr(now, k - now);

		now = k + 1;

		if (var == "") {
			int j = 0;
			while (j < val.size()) {
				if (val[j] == '\\') j++;
				var += val[j++];
			}
		}
		tokenRegex.push_back({ var, val });
		gSymbolTable.addSymbol(var, true);

		// 跳过一条正则后面的无效字符
		while (isIgnore(content[now])) now++;
	}
}

void Lex::formDFA() {
	// 各Token优先级
	unordered_map<string, int> prio;

	// 生成 NFA
	NFAPtr nfaP = make_shared<NFA>(NFA());

	for (int i = 0; i < tokenRegex.size(); i++) {
		nfaP->formRegular(tokenRegex[i].second, tokenRegex[i].first);
		prio[tokenRegex[i].first] = i;
	}

	// NFA to DFA
	dfaP = make_shared<DFA>(DFA(nfaP, prio));
}

void Lex::loadTarget(string fname) {
	ifstream fin(fname, fstream::in);
	tarText.assign(istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
}

Token Lex::nextToken(bool& isend) {
	string ts = "";
	int state = 0;
	while (tarText[ptr] == ' ' || tarText[ptr] == '\n' || tarText[ptr] == '\t') ptr++;
	int nptr = ptr;
	while (nptr < tarText.size()) {
		int nextS = dfaP->next(state, tarText[nptr]);
		if (nextS == -1) {
			ts = tarText.substr(ptr, nptr - ptr);
			ptr = nptr;
			return Token(ts, dfaP->nodes[state]->state);
		}
		else {
			nptr++;
			state = nextS;
		}
	}
	isend = true;
	ts = tarText.substr(ptr, nptr - ptr);
	ptr = nptr;
	return Token(ts, dfaP->nodes[state]->state);
}
