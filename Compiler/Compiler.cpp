#pragma once

#include <iostream>
#include "common.h"
#include "lex.h"
#include "yacc.h"
#include "parser.h"

int main()
{
	cout << "------  start   ------\n";

	Lex lex("TestLexical");

	cout << "--- lex parsing finished ---\n";

	lex.loadTarget("TestCode");

	cout << "--- code loading finished ---\n";

	Yacc yacc("TestGrammar");

	cout << "--- yacc parsing finished ---\n";

	Parser parser(lex, yacc, "outputtest.dot");

	cout << "--- start parsing ---\n\n\n";

	parser.parse();

	cout << "--- end parsing ---\n\n\n";



	//bool isend = false;
	//while (!isend) {
	//	Token token = lex.nextToken(isend);
	//	cout << token.type << "\t\t" << token.content << '\n';
	//}
}