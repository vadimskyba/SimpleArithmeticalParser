#include "stdafx.h"
#include "MathParser.h"
#include <assert.h>
#include <iostream>

#define nOPERATORS 4	// Number of supported operators
char const operators[] = { '*', '/', '+', '-' };	// Sequence of supported operators by priority

using namespace std;

CMathParser::CMathParser()
{
}

CMathParser::~CMathParser()
{
}

bool CMathParser::DoExpression(string str, double &result)
{
	for (size_t i = 0; i < str.length(); i++)	// Removing all white spaces and tabs from str
	{
		if (str[i] == ' ' || str[i] == '\t')
		{
			str.erase(i, 1);
			i--;
		}
	}

	double expressionResult;

	if (!ProcessExpression(str.c_str(), str.size(), expressionResult))
		return false;

	result = expressionResult;
	return true;
}

bool CMathParser::ProcessExpression(const char *str, const int nStrLen, double &result)
{
	// sequenses of parsed numbers and operations
	vector <double> numbers;
	vector <char> operations;

	// current position of str
	int iCurStrPos = 0;

	while (iCurStrPos < nStrLen)
	{
		SECTION_TYPE secType;

		if (!GetSectionType(str + iCurStrPos, nStrLen - iCurStrPos, &secType))
			return false; // Invalid expression

		assert(iCurStrPos + secType.nSecStrLen <= nStrLen);
		if (iCurStrPos + secType.nSecStrLen < nStrLen) // Current section length is less than nStrLen
		{
			if (!CheckNextSectionType(str + iCurStrPos, nStrLen - iCurStrPos, &secType))
				return false; // Invalid expression
		}

		switch (secType.type)
		{
		case SECTION_TYPE::brackets:
		{
			double inResult;
			if (!ProcessExpression(str + iCurStrPos + 1, secType.nSecStrLen - 2, inResult))
				return false;

			numbers.push_back(inResult);
			iCurStrPos += secType.nSecStrLen;
			break;

		}

		case SECTION_TYPE::number:
		{
			numbers.push_back(secType.numberValue);
			iCurStrPos += secType.nSecStrLen;
			break;
		}

		default:
			return false; // Internal error
		}

		if (iCurStrPos < nStrLen) // not the end of expression?
		{
			// Next char must be an operation
			if (!CheckProvidedOperation(str[iCurStrPos]))
				return false; // Expression error

			operations.push_back(str[iCurStrPos]);
			iCurStrPos++;
		}
	}

	double bracketsResult;
	if (!ProcessOperations(numbers, operations, bracketsResult))
		return false; // Invalid expression

	assert(numbers.size() == 0);
	assert(operations.size() == 0);

	result = bracketsResult;

	return true;
}

bool CMathParser::ProcessOperations(vector <double> &numbers, vector <char> &operations, double &result)
{
	if (numbers.size() - 1 != operations.size())
		return false; // Invalid Expression: extra operator

	// Process operators by priority
	for (int iOperator = 0; iOperator < nOPERATORS; iOperator++)
	{
		int i = 0;
		while (i < operations.size())
		{
			if (operations[i] == operators[iOperator])
			{
				double result = ProcessOperator(operations[i], numbers[i], numbers[i + 1]);
				operations.erase(operations.begin() + i);
				numbers.erase(numbers.begin() + i);

				numbers[i] = result;
				continue;
			}

			i++;
		}
	}

	if (numbers.size() == 1 && operations.size() == 0)
	{
		result = numbers.back();
		numbers.pop_back();
		return true;
	}

	return false;
}

double CMathParser::ProcessOperator(char Operator, double arg1, double arg2)
{
	switch (Operator)
	{
	case '*': return arg1 * arg2;
	case '/': return arg1 / arg2;
	case '+': return arg1 + arg2;
	case '-': return arg1 - arg2;
	}

	return false;
}

bool CMathParser::GetSectionType(const char *str, const int nStrLen, SECTION_TYPE *secType)
{
	int iNumStrPos = 0;
	int numSign = 1;

	if (str[iNumStrPos] == '-') // there is a sign '-' before number
	{
		numSign = -1;
		iNumStrPos++;
	}
	else if (str[iNumStrPos] == '+') // there is a sign '+' before number
	{
		iNumStrPos++;
	}

	if (str[iNumStrPos] >= '0' && str[iNumStrPos] <= '9') // does it a number?
	{
		int nNumStrLen;
		double numValue;
		if (!GetNumberValue(str + iNumStrPos, nStrLen - iNumStrPos, nNumStrLen, numValue))
			return false; // Invalid expression

		secType->type = SECTION_TYPE::number;
		secType->nSecStrLen = nNumStrLen + iNumStrPos;
		secType->numberValue = numValue * numSign;
	}
	else if (str[0] == '(')	// does it a bracket?
	{
		// Finding the position of close bracket
		int iClosedBrackedPos;
		if (!FindBracketClosePosition(str, nStrLen, iClosedBrackedPos))
			return false; // Invalid expression

		secType->type = SECTION_TYPE::brackets;
		secType->nSecStrLen = iClosedBrackedPos + 1;

	}
	else // not a number nor open bracket
	{
		return false; // Invalid expression
	}

	return true;
}

bool CMathParser::GetNumberValue(const char *str, const int nStrLen, int &nNumStrLen, double &number)
{
	if (!(str[0] >= '0' && str[0] <= '9'))
	{
		// error: not a number
		return false;
	}

	int nNumberStrLen = strspn(str, ".0123456789");

	if (nNumberStrLen > nStrLen) // error: the length of the number is more than nStrLen
	{
		return false;
	}

	string numberStr(str, 0, nNumberStrLen);
	number = atof(str);
	nNumStrLen = nNumberStrLen;

	return true;
}

bool CMathParser::FindBracketClosePosition(const char *str, const int nStrLen, int &iClosedBrackedPos)
{
	if (str[0] != '(')
		return false; // Error: the first char must be an open bracket

	int nOpenBrackets = 1;
	int curStrPos = 1;

	for (curStrPos = 1; curStrPos < nStrLen; curStrPos++)
	{
		if (str[curStrPos] == '(')
			nOpenBrackets++;
		else if (str[curStrPos] == ')')
			nOpenBrackets--;

		if (nOpenBrackets == 0) // The first open bracket was closed at curStrPos
			break;
	}

	assert(nOpenBrackets >= 0);

	if (nOpenBrackets > 0)
		return false; // Invalid expression: closing bracket not found

	if (curStrPos == 1)
		return false; // Invalid expression: empty brackets

	iClosedBrackedPos = curStrPos;
	return true;
}

bool CMathParser::CheckNextSectionType(const char *str, const int nStrLen, SECTION_TYPE *curSecType)
{
	assert(curSecType->nSecStrLen <= nStrLen);

	char nextChar = str[curSecType->nSecStrLen];

	switch (curSecType->type)
	{
		case SECTION_TYPE::number:
		case SECTION_TYPE::brackets:
			// the next SECTION_TYPE must be an operation
			return CheckProvidedOperation(nextChar);

		case SECTION_TYPE::operation:
			// the next SECTION_TYPE must be a number or brackets
			return ((nextChar >= '0' && nextChar <= '9') || nextChar == '(');

		default:
			assert(false); // error, unknown SECTION_TYPE
			break;
	}

	return false; // Expression error
}

bool CMathParser::CheckProvidedOperation(char operation)
{
	for (int i = 0; i < nOPERATORS; i++)
	{
		if (operators[i] == operation)
			return true;
	}

	return false;
}
