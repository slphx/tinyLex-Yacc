#include "Rule.h"

bool Rule::isEps() const {
	return expansions.size() == 0;
}

void Rule::append(int i, int s) {
	expansions[i].push_back(s);
}

int Rule::addExpansion() {
	expansions.push_back({});
	return expansions.size() - 1;
}

void Rule::print() {
	cout << gSymbolTable[origin] << " -> \n";

	for (auto expansion : expansions) {
		cout << '\t';
		for (auto elem: expansion)
			cout << gSymbolTable[elem] << " ";
		cout << '\n';
	}

}

void Rule::print(int index, int dot) {
	cout << gSymbolTable[origin] << " -> ";

	for (int i = 0; i < expansions[index].size(); i++) {
		if (i == dot) cout << "·";
		cout << gSymbolTable[expansions[index][i]] << " ";
	}
		
	cout << '\n';
}