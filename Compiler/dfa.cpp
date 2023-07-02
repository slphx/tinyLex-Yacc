#include "dfa.h"

DFA::DFA(NFAPtr nfaP, unordered_map<string, int>& prio) {
	// NFA DFA 节点映射关系	
	vector<setPtr> ndMap;
	// 待处理栈
	stack<int> waitSet;

	// 加入初始节点
	set<int> s;
	s.insert(0);
	nfaP->closure(s);
	ndMap.push_back(make_shared<set<int>>(s));
	nodes.push_back(make_shared<DFANode>(DFANode(0)));
	edges.push_back(make_shared<DFAEdge>(DFAEdge()));
	waitSet.push(0);

	while (!waitSet.empty()) {
		// 取出一个待处理节点
		int nowIndex = waitSet.top();
		waitSet.pop();

		// 遍历全部出边 统计全部出边情况
		unordered_map<char, setPtr> charNext;
		for (auto i : *ndMap[nowIndex]) {
			for (auto j : nfaP->edges[i]->trans) {
				if (charNext.find(j.first) == charNext.end())
					charNext[j.first] = make_shared<set<int>>();
				for (auto k : j.second) charNext[j.first]->insert(k);
			}
		}

		// 对每个出边字符，计算到达节点闭包
		for (auto j : charNext) {
			nfaP->closure(*j.second);
				
			// 判断是否为新节点
			int isExist = -1;
			for (int k = 0; k < ndMap.size(); k++) {
				if (*ndMap[k] == *j.second) isExist = k;
			}
				
			// 如果是新节点 则生成DFA节点，并加入waitSet
			if (isExist == -1) {
				ndMap.push_back(j.second);

				// 判断DFANode是否为接受状态
				string dfas = "";
				for (int k : *j.second) {
					string nfas = nfaP->nodes[k]->state;
					if (nfas != "" && (dfas == "" || prio[nfas] < prio[dfas])) {
						dfas = nfas;
					}
				}

				nodes.push_back(make_shared<DFANode>(DFANode(idCount, dfas)));
				edges.push_back(make_shared<DFAEdge>(DFAEdge()));
				edges[nowIndex]->addTrans(j.first, idCount);
				waitSet.push(idCount++);
			}
			// 如果是已经存在的节点，则添加边
			else {
				edges[nowIndex]->addTrans(j.first, isExist);
			}
		}
	}

}

void DFA::print() {
	for (int i = 0; i < edges.size(); i++) {
		cout << "Node " << i << ": " << nodes[i]->state << '\n';
		edges[i]->print();
		cout << '\n';
	}
}

int DFA::next(int state, char ch) {
	if (edges[state]->trans.find(ch) == edges[state]->trans.end()) return -1;
	return edges[state]->trans[ch];
}