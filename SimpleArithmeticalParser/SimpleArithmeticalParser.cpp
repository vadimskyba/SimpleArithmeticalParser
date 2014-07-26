// SimpleArithmeticalParser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include "MathParser.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "Hi, this is a simple arithmetical parser." << endl;
	cout << "Please, enter arithmetical expression like: 10.5*(2+3*(-4+3))-2*(2+3)" << endl;
	cout << "To quit the program, please type 'quit'." << endl << endl;

	CMathParser parser;

	bool bExit = false;
	while (!bExit)
	{
		string str;

		cout << "Expression: ";
		getline(cin, str);
		
		if (str == "quit")
		{
			bExit = true;
			continue;
		}

		double result;

		if (parser.DoExpression(str.c_str(), result))
		{
			cout << "Result: " << result << endl;
		}
		else
		{
			cout << "Expression error. Please try again." << endl;
		}
	}

	cout << "bye";

	return 0;
}
