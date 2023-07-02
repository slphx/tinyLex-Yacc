#pragma once
#include "common.h"
#include <utility>

enum action { shift, reduce };
struct Action {
	action act;
	// 如果是移进 则state表示移进目标项集id 如果是规约 则rIndex eIndex表示规约的rule
	int state;

	int rIndex;
	int eIndex;

	Action() : act(shift), state(0), rIndex(0), eIndex(0) { }
	Action(action a, int s, int r, int e) : act(a), state(s), rIndex(r), eIndex(e) {}
	Action(const Action& a) :act(a.act), state(a.state), rIndex(a.rIndex), eIndex(a.eIndex) {}
};

class ParsingTable {
public:
	ParsingTable() {}

	// Set id 状态下 接受 symbol 查询 Action
	unordered_map<pair<int, symbol>, Action, PairHash> table;
	
	bool insertAction(int i, symbol s, Action a) {
		if (table.find({ i,s }) == table.end()) {
			table[{i, s}] = a;
			return true;
		}
		else {
			return false;
		}
	}

	bool insertAction(int i, set<symbol> s, Action a) {
		int n = s.size();
		for (symbol q : s) {
			if (table.find({ i,q }) == table.end()) {
				table[{i, q}] = a;
			}
			else {
				n--;
			}
		}
		if (n == 0) {
			return false;
		}
		else return true;
	}

	bool ifHasNext(int i, symbol s){
		if (table.find({ i,s }) == table.end()) return false;
		return true;
	}
	Action next(int i, symbol s) {
		assert(table.find({ i,s }) != table.end());
		return table[{i, s}];
	}

	void print() {
		cout << "--------------- parsing table ---------------\n";
		for (auto p : table) {
			cout << p.first.first << '\t' << gSymbolTable[p.first.second] << '\t';
			if (p.second.act == shift) cout << "shift\t" << p.second.state << '\n';
			else cout << "reduce\t" << gSymbolTable[p.second.state] << '\n';
		}
	}
};