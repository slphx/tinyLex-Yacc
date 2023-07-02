#include "Grammar.h"

void Grammar::addRule(const rulePtr& r) {
	rules.push_back(r);
	allNSymbols->insert(r->origin);
}

Grammar::Grammar(symbol s) :start(s) {
	allNSymbols = make_shared<set<int>>(set<int>());
	nullSet = make_shared<set<int>>(set<int>());
}

void Grammar::calculate() {
	
	// 初始化first、follow 非终结符的First中加入自身
	for (auto sym : *allNSymbols) {
		first[sym] = make_shared<set<int>>(set<int>());
		follow[sym] = make_shared<set<int>>(set<int>());
	}
	// 不动点标记
	bool isChanged = true;
	// 计算First集
	while (isChanged) {
		isChanged = false;
		for (auto r : rules) {
			for (auto e : r->expansions) {
				if (e.size() > 0)
					if (gSymbolTable.isTerminal(e[0]))
					{
						if (updateSet(first[r->origin], e[0])) isChanged = true;
					}
					else {
						if (updateSet(first[r->origin], first[e[0]])) isChanged = true;
					}
					
				for (int i = 0; i < e.size(); i++) {
					// 右侧符号可以全为空，则origin也可以是空
					if (i == e.size() && ifEps(e[i])) {
						if (updateSet(nullSet, r->origin)) isChanged = true;
					}
					// 如果产生式中某个s左侧均可为空，则origin的First集合中添加First[s]集合
					else if (ifEps(e[i])) {
						if (gSymbolTable.isTerminal(e[i + 1]))
						{
							if (updateSet(first[r->origin], e[i + 1])) isChanged = true;
						}
						else {
							if (updateSet(first[r->origin], first[e[i + 1]])) isChanged = true;
						}
					}
					else break;
				}
			}

		}
	}

	// 计算Follow集 增广符号的follow加入$end
	follow[rules[0]->origin]->insert(gSymbolTable.endMark);
	isChanged = true;
	while (isChanged) {
		isChanged = false;
		for (auto r : rules) {
			for (auto e : r->expansions) {
				if (e.size() > 0) {
					// A->aBb Follow(B) 包括 First(b)
					for (int i = 0; i < e.size() - 1; i++) {
						if (!gSymbolTable.isTerminal(e[i])) {
							if (gSymbolTable.isTerminal(e[i + 1])) {
								if (updateSet(follow[e[i]], e[i + 1])) isChanged = true;
							}
							else {
								if (updateSet(follow[e[i]], first[e[i + 1]])) isChanged = true;
							}
						}
					}
					// A->aBb First(b)包括epsilon 则 Follow(B) 包括 Follow(A)
					int i = e.size() - 1;
					if (!gSymbolTable.isTerminal(e[i])) {
						if (updateSet(follow[e[i]], follow[r->origin])) isChanged = true;
					}
					while (i > 0 && !gSymbolTable.isTerminal(e[i]) && ifEps(e[i])) {
						i--;
						if (i > 0 && !gSymbolTable.isTerminal(e[i])) {
							if (updateSet(follow[e[i]], follow[r->origin])) isChanged = true;
						}
					}
				}
			}
		}
	}
}

bool Grammar::ifEps(symbol s) {
	if (nullSet->find(s) == nullSet->end()) return false;
	return true;
}

bool Grammar::updateSet(setPtr t, setPtr s) {
	if (s->size() == 0) return false;
	int len = t->size();
	t->insert(s->begin(), s->end());
	return t->size() != len;
}

bool Grammar::updateSet(setPtr t, symbol s) {
	int len = t->size();
	t->insert(s);
	return t->size() != len;
}

void Grammar::ruleIndex(int i, int j) {
	ruleIndexMap[i] = j;
}

int Grammar::ruleIndex(int i) {
	if (ruleIndexMap.find(i) != ruleIndexMap.end()) return ruleIndexMap[i];
	return -1;
}

void Grammar::printSets() {
	cout << "----------first----------" << endl;
	for (auto pr : first) {
		cout << gSymbolTable[pr.first] << ":\n" << endl;
		for (auto sym : *pr.second) {
			cout << '\t' << gSymbolTable[sym];
		}
		cout << '\n';
	}

	cout << "----------follow----------" << endl;
	for (auto pr : follow) {
		cout << gSymbolTable[pr.first] << ":\n" << endl;
		for (auto sym : *pr.second) {
			cout << '\t' << gSymbolTable[sym];
		}
		cout << '\n';
	}

	cout << "-----------nullable-----------" << endl;
	for (auto sym : *nullSet) {
		cout << gSymbolTable[sym] << " ";
	}
	cout << '\n';
}

void Grammar::printRules() {
	for (auto rs : rules) {
		rs->print();
	}
}