#pragma once
#include "common.h"
#include "grammar.h"
#include "parsingTable.h"

using namespace std;

class Item;
class ItemSet;
class LALRParser;
typedef shared_ptr<Item> ItemPtr;
typedef shared_ptr<ItemSet> ItemSetPtr;
typedef shared_ptr<LALRParser> LALRParserPtr;


class Item {
public:
	Item(rulePtr r, int i, int d, GrammarPtr p) :rp(r), index(i), dot(d), grammarP(p) {}

	// 所属文法
	GrammarPtr grammarP;

	// 产生式
	rulePtr rp;
	int index;

	// 当前dot标识位置
	int dot;
	// 后继符号
	set<symbol> successors;

	// 返回dot后下一个符号，约定-3为可规约
	symbol next() {
		if (dot == rp->expansions[index].size()) return -3;
		return rp->expansions[index][dot];
	}

	// 计算后继符号集
	void succ(set<symbol>& se) {
		int n = rp->expansions[index].size();
		if (dot == n) {
			se.clear();
		}
		else {
			int d = dot + 1;

			while (1) {
				if (d == n) {
					se.insert(successors.begin(), successors.end());
					break;
				}
				if (!gSymbolTable.isTerminal(rp->expansions[index][d])) {
					for (auto i : *grammarP->first[rp->expansions[index][d]]) {
						se.insert(i);
					}
					if (grammarP->ifEps(rp->expansions[index][d]))
						d++;
					else break;
				}
				else {
					se.insert(rp->expansions[index][d]);
					break;
				}
			}
		}
	}

	bool addSuc(symbol s) {
		int l = successors.size();
		successors.insert(s);
		if (l != successors.size()) return true;
		else return false;
	}
	bool addSuc(set<symbol>& s) {
		int l = successors.size();
		successors.insert(s.begin(), s.end());
		if (l != successors.size()) return true;
		else return false;
	}
	
	bool operator==(const Item& ip) const {		// 用于比较两个Item是否相等
		return (rp == ip.rp) && (index == ip.index) && (dot == ip.dot);
	}

	void print() {
		rp->print(index, dot);
		cout << "\t";
		for (symbol s : successors) {
			cout << gSymbolTable[s] << ' ';
		}
		cout << '\n';
	}
};

class ItemSet {
public:
	ItemSet(ItemPtr i, GrammarPtr p, int n) {
		grammarP = p;
		items.push_back(i);
		Id = n;
	}
	ItemSet(GrammarPtr p, int i) {
		grammarP = p;
		Id = i;
	}

	// 所属文法
	GrammarPtr grammarP;

	// 项集
	vector<ItemPtr> items;

	set<tuple<int, int, int>> id;
	int Id;

	// 拓展完整
	void expand();

	void addItem(rulePtr r, int i, int d, set<symbol>& s) {
		items.push_back(make_shared<Item>(Item(r, i, d, grammarP)));
		items[items.size() - 1]->addSuc(s);
	}

	void addSuc(int i, symbol s) {
		items[i]->successors.insert(s);
	}
	void addSuc(int i, set<symbol>& s) {
		items[i]->successors.insert(s.begin(), s.end());
	}

	void print() {
		for (int i = 0; i < items.size(); i++) {
			items[i]->print();
		}
	}
};

class LALRParser {
public:
	LALRParser() {}
	LALRParser(GrammarPtr gp);
	
	GrammarPtr grammarP;
	vector<ItemSetPtr> itemSets;	// 所有项集

	ParsingTable pt;

	// 将每个ItemSet 的 id 映射到 itemSets 的索引上
	unordered_map<set<tuple<int, int, int>>, set<int>, SetTupleHash> idMap;

	// 生成语法分析表
	void computeParsingTable();

	void print() {
		for (int i = 0; i < itemSets.size(); i++) {
			cout << "----------   " << i << "   ----------\n";
			itemSets[i]->print();
		}
		cout << '\n';
	}

	void printParsingTable() {
		pt.print();
	}

};
