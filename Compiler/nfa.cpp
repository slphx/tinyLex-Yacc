#include "nfa.h"

NFA::NFA() {
	nodes.push_back(make_shared<NFANode>(NFANode(0))); 
	edges.push_back(make_shared<NFAEdge>(NFAEdge()));
	idCount = 1;
}

unordered_map<char, int> NFA::priorityin = { {'*',1}, {'+',1}, {'?',1}, {'(',2}, {'.',3}, {'|',4}, {')',5}, {'$',6} };
unordered_map<char, int> NFA::priorityout = { {'*',1}, {'+',1}, {'?',1}, {'.',3}, {'|',4}, {'(',5}, {')',5}, {'$',6} };

bool NFA::isop(char ch) {
	if (ch == '*' || ch == '+' || ch == '?' || ch == '|' || ch == '(' || ch == ')' || ch == '$') return true;
	else return false;
}

void NFA::formRegular(string re, string state) {

	// re长度为1或2 特殊处理
	if (re.size() <= 2) {
		if (re.size() == 1 || re[0] == '\\') {
			char ch = re[re.size() - 1];
			string s = "";
			s += ch;
			int edId = idCount++;
			NFANodePtr ed = (make_shared<NFANode>(NFANode(edId, s)));
			nodes.push_back(ed);
			NFAEdgePtr edE = (make_shared<NFAEdge>(NFAEdge()));
			edges.push_back(edE);
			edges[0]->addTrans(ch, edId);

			return;
		}
	}

	stack<char> op;	// 操作符 栈
	stack<pair<int, int>> opn;	// 操作数 栈
	bool cflag = false;	// 用于判断 是否添加连接符 标记前一个输入是否为操作数
						// 当连续出现两个操作数或是括号，则需要在其间添加连接符
						
	re += '$';
	op.push('$');

	int i = 0;
	while (i < re.size()) {
		char ch = re[i];

		// 判断是否转义
		bool istrans = false;

		if (ch == '\\') {
			i++;  ch = re[i];
			istrans = true;
		}

		// 添加连接符
		if (cflag && (istrans || !isop(ch) || ch == '(')) {	
			check(op, opn, '.');
		}

		// 如果是操作符则尝试入栈，否则就作为操作数处理
		if (isop(ch) && !istrans) {
			check(op, opn, ch);
		}
		else {
			NFA partialNFA;

			// 处理 [] 解析
			if (ch == '[') {
				int bgId = idCount++;
				int edId = idCount++;
				pair<int, int> p(bgId, edId);
				NFANodePtr bg = (make_shared<NFANode>(NFANode(bgId)));
				NFANodePtr ed = (make_shared<NFANode>(NFANode(edId)));

				nodes.push_back(bg); nodes.push_back(ed);

				NFAEdgePtr bgE = (make_shared<NFAEdge>(NFAEdge())); 
				NFAEdgePtr edE = (make_shared<NFAEdge>(NFAEdge()));

				if (re[i + 1] == '^') {
					set<char> fset;
					i += 2;
					while (re[i] != ']') {
						if (re[i] == '\\') {
							i++;
							fset.insert(re[i]);
						}
						else if (re[i] == '-') {
							for (char c = re[i - 1] + 1; c <= re[i + 1]; c++) {
								fset.insert(c);
							}
							i++;
						}
						else fset.insert(re[i]);
						i++;
					}
					for (int t = 32; t < 127; t++) {
						if (fset.find((char)t) == fset.end()) bgE->addTrans((char)t, edId);
					}
				}
				else {
					i++;
					while (re[i] != ']') {
						if (re[i] == '-') {
							for (char c = re[i - 1] + 1; c <= re[i + 1]; c++) {
								bgE->addTrans(c, edId);
							}
							i++;
						}
						else bgE->addTrans(re[i], edId);
						i++;
					}
				}
				edges.push_back(bgE);
				edges.push_back(edE);
				opn.push(p);
			}
			// 其余字符
			else {
				int bgId = idCount++;
				int edId = idCount++;
				pair<int, int> p(bgId, edId);
				NFANodePtr bg = make_shared<NFANode>(NFANode(bgId));
				NFANodePtr ed = make_shared<NFANode>(NFANode(edId));

				nodes.push_back(bg); nodes.push_back(ed);

				NFAEdgePtr bgE = make_shared<NFAEdge>(NFAEdge());
				NFAEdgePtr edE = make_shared<NFAEdge>(NFAEdge());
				bgE->addTrans(ch, edId);
				
				edges.push_back(bgE);
				edges.push_back(edE);
				opn.push(p);
			}
		}
		if (!istrans && (ch == '(' || ch == '|')) cflag = false; else cflag = true;
		i++;
	}

	pair<int, int> p = opn.top();
	nodes[idCount - 1]->state = state;
	edges[0]->epsTrans.insert(p.first);
}

void NFA::check(stack<char>& op, stack<pair<int, int>>& opn, char o) {
	bool ifPush = true;
	while (priorityout[op.top()] <= priorityin[o]) {
		char optop = op.top(); op.pop();
		if (optop == '$' && o == '$') {
			ifPush = false; break;
		}
		else if (optop == '*') {
			pair<int, int> repeat = opn.top(); opn.pop();

			int bgId = idCount++;
			int edId = idCount++;
			pair<int, int> connect(bgId, edId);
			opn.push(connect);

			NFANodePtr bg = (make_shared<NFANode>(NFANode(bgId)));
			NFANodePtr ed = (make_shared<NFANode>(NFANode(edId)));

			nodes.push_back(bg); nodes.push_back(ed);

			NFAEdgePtr bgE = make_shared<NFAEdge>(NFAEdge());
			NFAEdgePtr edE = make_shared<NFAEdge>(NFAEdge());

			bgE->epsTrans.insert(repeat.first);
			bgE->epsTrans.insert(edId);
			edges[repeat.second]->epsTrans.insert(edId);
			edges[repeat.second]->epsTrans.insert(repeat.first);
			edges.push_back(bgE);
			edges.push_back(edE);
		}
		else if (optop == '+') {
			pair<int, int> repeat = opn.top(); opn.pop();

			int bgId = idCount++;
			int edId = idCount++;
			pair<int, int> connect(bgId, edId);
			opn.push(connect);

			NFANodePtr bg = (make_shared<NFANode>(NFANode(bgId)));
			NFANodePtr ed = (make_shared<NFANode>(NFANode(edId)));

			nodes.push_back(bg); nodes.push_back(ed);

			NFAEdgePtr bgE = make_shared<NFAEdge>(NFAEdge());
			NFAEdgePtr edE = make_shared<NFAEdge>(NFAEdge());

			bgE->epsTrans.insert(repeat.first);
			edges[repeat.second]->epsTrans.insert(edId);
			edges[repeat.second]->epsTrans.insert(repeat.first);

			edges.push_back(bgE);
			edges.push_back(edE);
		}
		else if (optop == '?') {
			pair<int, int> repeat = opn.top(); opn.pop();

			int bgId = idCount++;
			int edId = idCount++;
			pair<int, int> connect(bgId, edId);
			opn.push(connect);

			NFANodePtr bg = (make_shared<NFANode>(NFANode(bgId)));
			NFANodePtr ed = (make_shared<NFANode>(NFANode(edId)));

			nodes.push_back(bg); nodes.push_back(ed);

			NFAEdgePtr bgE = make_shared<NFAEdge>(NFAEdge());
			NFAEdgePtr edE = make_shared<NFAEdge>(NFAEdge());

			bgE->epsTrans.insert(repeat.first);
			bgE->epsTrans.insert(edId);
			edges[repeat.second]->epsTrans.insert(edId);

			edges.push_back(bgE);
			edges.push_back(edE);
		}
		else if (optop == '.') {
			pair<int, int> connect;
			pair<int, int> and2 = opn.top(); opn.pop();
			pair<int, int> and1 = opn.top(); opn.pop();
			connect.first = and1.first;
			connect.second = and2.second;
			edges[and1.second]->epsTrans.insert(and2.first);
			opn.push(connect);
		}
		else if (optop == '|') {
			pair<int, int> or2 = opn.top(); opn.pop();
			pair<int, int> or1 = opn.top(); opn.pop();

			int bgId = idCount++;
			int edId = idCount++;
			std::pair<int, int> connect(bgId, edId);
			opn.push(connect);

			NFANodePtr bg = (make_shared<NFANode>(NFANode(bgId)));
			NFANodePtr ed = (make_shared<NFANode>(NFANode(edId)));

			nodes.push_back(bg); nodes.push_back(ed);

			NFAEdgePtr bgE = make_shared<NFAEdge>(NFAEdge());
			NFAEdgePtr edE = make_shared<NFAEdge>(NFAEdge());

			bgE->epsTrans.insert(or1.first);
			bgE->epsTrans.insert(or2.first);
			edges[or1.second]->epsTrans.insert(connect.second);
			edges[or2.second]->epsTrans.insert(connect.second);
			edges.push_back(bgE);
			edges.push_back(edE);
		}
		else if (optop == '(' && o == ')') {
			ifPush = false; break;
		}
	}
	if (ifPush) op.push(o);
}

void NFA::closure(set<int>& s) {
	stack<int> st;
	for (auto i : s) st.push(i);
	while (!st.empty()) {
		int i = st.top(); st.pop();
		for (auto k : edges[i]->epsTrans) {
			if (s.find(k) == s.end()) {
				s.insert(k);
				st.push(k);
			}
		}
	}
}


void NFA::print() {
	for (int i = 0; i < edges.size(); i++) {
		cout << "Node " << i << ": " << nodes[i]->state << '\n';
		edges[i]->print();
		cout << '\n';
	}
}