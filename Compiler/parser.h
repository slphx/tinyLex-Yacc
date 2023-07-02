#pragma once
#include "common.h"
#include "lex.h"
#include "yacc.h"

class Node {
public:
	Token token;
	int id;
	vector<int> sons;
	int treeId;

	static int idCount;
	Node(Token t) :token(t) {
		id = idCount++;
	}

};
int Node::idCount = 0;
typedef shared_ptr<Node> NodePtr;

class ParseTree {
public:
	vector<NodePtr> tree;
	int count = 1;
	ofstream out;

	ParseTree(string s) {
		out.open(s);
	}

	void addNode(NodePtr np) {
		tree.push_back(np);
	}

	void printout(int start) {
		out << "digraph SyntaxTree {\n";
		out << "\tnode [shape = box];\n";

		tree[start]->treeId = count;
		out << '\t' << count++ << '\t' << "[label = \"" << tree[start]->token.type << "\"];\n";

		trans(start);
		out << "}";
	}
	void trans(int i) {
		int index;
		for (int j = tree[i]->sons.size() - 1; j >= 0; j--) {
			index = tree[i]->sons[j];
			// 依次生成节点
			tree[index]->treeId = count;
			out << '\t' << count++ << '\t' << "[label = \"" << tree[index]->token.type << "\"];\n";
		}
		for (int j = tree[i]->sons.size() - 1; j >= 0; j--) {
			index = tree[i]->sons[j];
			// 依次生成边
			out << '\t' << tree[i]->treeId << "\t->\t" << tree[index]->treeId << ";\n";
		}
		for (int j = tree[i]->sons.size() - 1; j >= 0; j--) {
			index = tree[i]->sons[j];
			trans(index);
		}
	}
};



class Parser {
public:
	LexPtr lex;
	YaccPtr yacc;
	ParseTree pt;

	Parser(Lex& l, Yacc& y, string fname) :pt(fname) {
		lex = make_shared<Lex>(l);
		yacc = make_shared<Yacc>(y);
	}

	void parse() {

		bool isend = false;
		stack<NodePtr> tokenStack;
		stack<int> stateStack;
		stack<NodePtr> symbolStack;
		NodePtr nowNode;
		int nowSymbol;
		int state = 0;
		stateStack.push(0);

		while (!stateStack.empty()) {

			state = stateStack.top();

			// 如果tokenStack有Token 则从tokenStack中取 否则从lex中取
			if (tokenStack.empty()) {
				if (!isend) {
					nowNode = make_shared<Node>(Node(lex->nextToken(isend)));
					pt.addNode(nowNode);
					//out << '\t' << nowNode->id << '\t' << "[label = \"" << nowNode->token.type << "\"];\n";
				}
				else {
					nowNode = make_shared<Node>(Node(Token("$end")));
					pt.addNode(nowNode);
				}
			}
			else {
				nowNode = tokenStack.top(); tokenStack.pop();
			}
			nowSymbol = gSymbolTable[nowNode->token.content];

			// 在当前状态下的Action
			Action action = yacc->parser->pt.next(state, nowSymbol);
			
			if (action.act == shift) {
				// 如果是移进
				stateStack.push(action.state);
				symbolStack.push(nowNode);

			}
			else {
				// 如果是规约

				// 找到规约的产生式
				rulePtr r = yacc->grammarP->rules[action.rIndex];

				if (r->origin == gSymbolTable.startMark && nowNode->token.type == "$end") break;

				// 构建新的节点
				NodePtr newNode;
				newNode = make_shared<Node>(Node(gSymbolTable[r->origin]));
				tokenStack.push(nowNode);
				tokenStack.push(newNode);
				pt.addNode(newNode);
				//out << '\t' << newNode->id << '\t' << "[label = \"" << newNode->token.type << "\"];\n";
				

				for (int i = 0; i < r->expansions[action.eIndex].size(); i++) {
					// 构建节点连接情况

					pt.tree[newNode->id]->sons.push_back(symbolStack.top()->id);
					//out << '\t' << newNode->id << "\t->\t" << symbolStack.top()->id << ";\n";

					stateStack.pop();
					symbolStack.pop();
				}
			}
		}

		pt.printout(symbolStack.top()->id);
	}

};