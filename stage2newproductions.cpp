// New productions added in stage 1

#include "stage2.h"

void Stage::ExecStmts()
{
	try {
		while (token != "end")
		{
			ExecStmt();

			NextToken();
		}
	} catch (baseException e) {
		throw;
	}
}

void Stage::ExecStmt()
{
	try {		
		if (isNonKeyID(token)) 
			AssignStmt();

		else if (token == "read")
			ReadStmt();

		else if (token == "write")
			WriteStmt();

		else
			throw syntaxError("NonKeyId, \"read\", or \"write\" expected.", getLine()); //error

	} catch (baseException e) {
		throw;
	}
}

void Stage::AssignStmt()
{
	try {
		if (!isNonKeyID(token))
			throw syntaxError("NonKeyId expected", getLine());

		PushOperand(token);

		if (NextToken() != ":=")
			throw syntaxError("expected ':='", getLine());

		PushOperator(token); // should be ":="

		NextToken();

		Express();

		if (token != ";")
			throw syntaxError("expected ';'.", getLine());

		Code(PopOperator(), PopOperand(), PopOperand());

	} catch (baseException e) {
		throw;
	}
}

void Stage::Express()
{
	try {
		Term();

		if (isRelOp(token))
			Expresses();

	} catch (baseException e) {
		throw;
	}
}

void Stage::Term()
{
	try {
		Factor();

		Terms();

	} catch (baseException e) {
		throw;
	}
}

void Stage::Factor()
{
	try {
		Part();

		Factors();

	} catch (baseException e) {
		throw;
	}
}

void Stage::Part()
{
	try {
		if (token == "not")
		{
			if (NextToken() == "(")
			{
				Express();

				Code("not", PopOperand());
			}

			else if (isBoolean(token))
			{
				if (token == "true")
					PushOperand("false");
				else
					PushOperand("true");

				NextToken();
			}

			else if (isNonKeyID(token))
			{
				Code("not", token);

				NextToken();
			}
			else
				throw syntaxError("'(', boolean, or non keyword id expected", getLine());
		}

		else if (token == "+")
		{
			if (NextToken() == "(")
			{
				Express();

				//Code("not", PopOperand());
			}

			else if (isInteger(token) || isNonKeyID(token))
			{
				PushOperand(token);

				NextToken();
			}

			else
				throw syntaxError("'(', integer, or non keyword id expected", getLine());
		}

		else if (token == "-")
		{
			if (NextToken() == "(")
			{
				Express();

				Code("neg", PopOperand());
			}

			else if(isInteger(token))
			{
				PushOperand("-" + token);

				NextToken();
			}

			else if (isNonKeyID(token))
			{
				Code("neg", token);

				NextToken();
			}
			else
				throw syntaxError("'(', integer, or non keyword id expected", getLine());
		}

		else if (isInteger(token) || isBoolean(token) || isNonKeyID(token))
		{
			PushOperand(token);

			NextToken();
		}

		else if (token == "(")
		{
			NextToken();

			Express();

			if (token != ")")
				throw syntaxError("')' expected", getLine());

			NextToken();
		}

		else
			throw syntaxError("Invalid token", getLine());

	} catch (baseException e) {
		throw;
	}
}

void Stage::Factors()
{
	if (isMultLevelOp(token))
		try {
			string x;

			x = MultLevelOp(token);

			PushOperator(x);

			NextToken();
	
			Part();

			Code(PopOperator(), PopOperand(), PopOperand());

			Factors();

		} catch (baseException e) {
			throw;
		}
}

void Stage::Terms()
{
	if (isAddLevelOp(token))
		try {
				string x;

				x = AddLevelOp(token);

				PushOperator(x);

				NextToken();
				
				Factor();

				Code(PopOperator(), PopOperand(), PopOperand());

				Terms();
		
		} catch (baseException e) {
			throw;
		}
}

void Stage::Expresses()
{
	if (isRelOp(token))
		try {
			string x;

			x = RelOp(token);

			PushOperator(x);

			NextToken();

			Term();

			Code(PopOperator(), PopOperand(), PopOperand());

			Expresses();

		} catch (baseException e) {
			throw;
		}

}

string Stage::RelOp(string check)
{
	vector<string>::iterator match;
	vector<string> symbols;

	symbols.push_back("=");
	symbols.push_back("<>");
	symbols.push_back("<=");
	symbols.push_back(">=");
	symbols.push_back("<");
	symbols.push_back(">");

	match = find(symbols.begin(), symbols.end(), check);

	if (match == symbols.end())
		throw syntaxError("no relative operator found", getLine());

	return *match;
}

string Stage::AddLevelOp(string check)
{
	vector<string>::iterator match;
	vector<string> symbols;

	symbols.push_back("+");
	symbols.push_back("-");
	symbols.push_back("or");

	match = find(symbols.begin(), symbols.end(), check);

	if (match == symbols.end())
		throw syntaxError("no addition level operator found", getLine());

	return *match;
}

string Stage::MultLevelOp(string check)
{
	vector<string>::iterator match;
	vector<string> symbols;

	symbols.push_back("*");
	symbols.push_back("div");
	symbols.push_back("mod");
	symbols.push_back("and");

	match = find(symbols.begin(), symbols.end(), check);

	if (match == symbols.end())
		throw syntaxError("no multiplication level operator found", getLine());

	return *match;
}

void Stage::ReadStmt()
{
	try {
		if (token != "read")
			throw syntaxError("keyword \"read\" expected", getLine());

		ReadList();

		if (NextToken() != ";")
			throw syntaxError("';' expected", getLine());

	} catch (baseException e) {
		throw;
	}
}

void Stage::ReadList()
{
	string x;

	try {
		if (NextToken() != "(")
			throw syntaxError("'(' expected.", getLine());

		NextToken();

		x = Ids(); 

		if (token != ")")
			throw syntaxError("',' or ')' expected after non-keyword identifier", getLine());

		Code("read", x);

	} catch (baseException e) {
		throw;
	}
}

void Stage::WriteStmt()
{
	try {
		if (token != "write")
			throw syntaxError("keyword \"write\" expected", getLine());

		WriteList();

		if (NextToken() != ";")
			throw syntaxError("';' expected.", getLine());

	} catch (baseException e) {
		throw;
	}
}

void Stage::WriteList()
{
	string x;

	try {
		if (NextToken() != "(")
			throw syntaxError("'(' expected.", getLine());

		NextToken();

		x = Ids();

		if (token != ")")
			throw syntaxError("',' or ')' expected after non-keyword identifier", getLine());		

		Code("write", x);

	} catch (baseException e) {
		throw;
	}
}

bool Stage::isRelOp(string check)
{
	vector<string> symbols;

	symbols.push_back("=");
	symbols.push_back("<>");
	symbols.push_back("<=");
	symbols.push_back(">=");
	symbols.push_back("<");
	symbols.push_back(">");

	return find(symbols.begin(), symbols.end(), check) != symbols.end();
}

bool Stage::isAddLevelOp(string check)
{
	vector<string> symbols;

	symbols.push_back("+");
	symbols.push_back("-");
	symbols.push_back("or");

	return find(symbols.begin(), symbols.end(), check) != symbols.end();
}

bool Stage::isMultLevelOp(string check)
{
	vector<string> symbols;

	symbols.push_back("*");
	symbols.push_back("div");
	symbols.push_back("mod");
	symbols.push_back("and");

	return find(symbols.begin(), symbols.end(), check) != symbols.end();
}
