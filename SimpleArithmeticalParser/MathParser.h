#pragma once

#include <string>
#include <vector>

class CMathParser
{
	struct SECTION_TYPE
	{
		enum { number, operation, brackets } type;
		int nSecStrLen;
		double numberValue; // contains namber if (secType == number)
	};
	
public:
	CMathParser();
	~CMathParser();

	bool DoExpression(std::string str, double &result);

protected:
	bool ProcessExpression(const char *str, const int nStrLen, double &result);
	bool ProcessOperations(std::vector <double> &numbers, std::vector <char> &operations, double &result);
	double ProcessOperator(char Operator, double arg1, double arg2);
	bool GetSectionType(const char *str, const int nStrLen, SECTION_TYPE *secType);
	bool GetNumberValue(const char *str, const int nStrLen, int &nNumStrLen, double &number);
	bool FindBracketClosePosition(const char *str, const int nStrLen, int &iClosedBrackedPos);
	bool CheckNextSectionType(const char *str, const int nStrLen, SECTION_TYPE *curSecType);
	bool CheckProvidedOperation(char operation);
};

