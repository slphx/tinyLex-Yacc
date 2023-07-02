#include "common.h"

SymbolTable gSymbolTable;

SymbolTable::SymbolTable()
{
	// 添加开始符号和结束符号
	table["$start"] = -2;
	table["$end"] = -1;
	rTable[-2] = { "$start", false };
	rTable[-1] = { "$end", true };  // #end是终结符

}

// 若已存在，则返回索引相反数
int SymbolTable::addSymbol(const string& s, bool isterminal)
{
	if (table.find(s) != table.end()) {
		return table[s];
	}
	int index = newIndex();
	table[s] = index;
	rTable[index] = { s, isterminal };
	return index;
}

int SymbolTable::newIndex()
{
	static int count = 0;
	return ++count;
}

bool SymbolTable::isTerminal(int index) {
	assert(rTable.find(index) != rTable.end());
	return rTable[index].isTerminal == true;
}

int SymbolTable::operator[](const string& s) {
	assert(table.find(s) != table.end());
	return table[s];
}

string SymbolTable::operator[](int index) {
	assert(rTable.find(index) != rTable.end());
	return rTable[index].name;
}