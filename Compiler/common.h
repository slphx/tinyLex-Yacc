#pragma once
#include <unordered_map>
#include <tuple>
#include <map>
#include <vector>
#include <set>
#include <stack>
#include <iostream>
#include <fstream>
#include <regex>
#include <assert.h>

using namespace std;

typedef int symbol;

class Token {
public:
	string type;
	string content;

	Token() :type(""), content("") {}
	Token(string s1, string s2) :type(s1), content(s2) {}
	Token(string s) :type(s), content(s) {}
};

class SymbolTable {
private:
	struct Symbol {
		string name;
		bool isTerminal;
	};

public:
	SymbolTable();

	const int startMark = -2;
	const int endMark = -1;

	// symbol 索引 index
	unordered_map<string, int> table;
	// index 索引 symbol
	unordered_map<int, Symbol> rTable;

	// 返回新索引值
	static int newIndex();

	int addSymbol(const string& s, bool terminal);
	bool isTerminal(int index);

	// 重写[]操作符，便于查询符号表
	int operator[](const string& s);
	string operator[](int index);

};

// 全局符号表
extern SymbolTable gSymbolTable;

struct SetTupleHash {
	size_t operator()(const set<tuple<int, int, int>>& key) const {
		size_t seed = 0;
		for (const auto& tuple : key) {
			seed += hash<int>{}(get<0>(tuple)) + hash<int>{}(get<1>(tuple)) + hash<int>{}(get<2>(tuple));
		}
		return seed;
	}
};

struct PairHash {
	template <class T1, class T2>
	size_t operator()(const pair<T1, T2>& pair) const {
		// 组合计算 T1 和 T2 的哈希值
		size_t hash1 = hash<T1>{}(pair.first);
		size_t hash2 = hash<T2>{}(pair.second);
		return hash1 ^ hash2; // 使用异或运算符进行组合
	}
};

struct TupleHash {
	template <class... Args>
	size_t operator()(const tuple<Args...>& tuple) const {
		size_t seed = 0;

		hash_combine(seed, std::get<0>(tuple));
		hash_combine(seed, std::get<1>(tuple));
		hash_combine(seed, std::get<2>(tuple));

		return seed;
	}

private:
	// 自定义的 hash_combine 函数
	template <typename T>
	void hash_combine(std::size_t& seed, const T& value) const {
		seed ^= std::hash<T>{}(value)+0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
};