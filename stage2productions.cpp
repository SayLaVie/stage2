//Submitted Version
#include "stage2.h"

// Production 1
void Stage::Prog() // token should be "program"
{
	try {
		if (token != "program")
			throw syntaxError("keyword \"program\" expected", getLine()); //process error: keyword "program expected"

		ProgStmt();

		if (token == "const") 
			Consts();

		if (token == "var") 
			Vars();

		if (token != "begin")
			throw syntaxError("keyword \"begin\" expected", getLine()); //process error: keyword "begin" expected

		BeginEndStmt();

		if (charac != END_OF_FILE)
			throw syntaxError("no text may follow \"end\"", getLine()); //process error: no text may follow "end"

	} catch (baseException e) {
		throw;
	}
}

// Production 2
void Stage::ProgStmt() // program should be "program"
{
	try {
		string x;

		if (token != "program")
			throw syntaxError("keyword \"program\" expected", getLine()); //process error: keyword "program" expected

		x = NextToken();

		if (!isNonKeyID(token))
			throw syntaxError("program name expected", getLine()); //process error: program name expected

		if (NextToken() != ";")
			throw syntaxError("semicolon expected", getLine()); // process error: semicolon expected

		NextToken();

		Insert(x, PROG_NAME, CONSTANT, x, NO, 0);

		// Emit Program Code here -- do we still need to call code above?
		// seems that the programname no longer needs to be in the symbol table
		EmitProgramCode(x);

	} catch (baseException e) {
		throw;
	}
}

// Production 3
void Stage::Consts() //token should be "const"
{
	try {
		if (token != "const")
			throw syntaxError("keyword \"const\" expected", getLine()); //process error: keyword "const" expected

		if (!isNonKeyID(NextToken()))
			throw syntaxError("non-keyword identifier must follow \"const\"", getLine()); //process error: non-keyword identifier must follow "const"

		ConstStmts();

	} catch (baseException e) {
		throw;
	}
}

// Production 4
void Stage::Vars() // token should be "var"
{
	try {
		if (token != "var")
			throw syntaxError("keyword \"var\" expected", getLine()); //process error: keyword "var" expected
		
		if (!isNonKeyID(NextToken()))
			throw syntaxError("non-keyword identifier must follow \"var\"", getLine()); //process error: non-keyword identifier must follow "var"

		VarStmts();

	} catch (baseException e) {
		throw;
	}
}

// Production 5
void Stage::BeginEndStmt() // token should be "begin"
{
	try {
		if (token != "begin")
			throw syntaxError("keyword \"begin\" expected", getLine()); //process error: keyword "begin" expected

		/*Some code here calling new productions*/
		NextToken();
		
		ExecStmts();

		if (token != "end")
			throw syntaxError("keyword \"end\" expected", getLine()); //process error: keyword "end" expected
		if (NextToken() != ".")
			throw syntaxError("period expected", getLine()); //process error: period expected

		Code("hlt");

		NextToken();


	} catch (baseException e) {
		throw;
	}
}

// Production 6
void Stage::ConstStmts() //token should be NON_KEY_ID
{
	try {
		string x, y;

		if (!isNonKeyID(token))
			throw syntaxError("non-keyword identifier expected", getLine()); //process error: non-keyword identifier expected

		x = token;

		if (NextToken() != "=")
			throw syntaxError("\"=\" expected", getLine()); //process error: "=" expected

		y = NextToken();

		if (!isLiteral(y) && !isNonKeyID(y))
			throw syntaxError("token to right of \"=\" illegal", getLine()); // Process error: token to right of "=" illegal
		
		if (y == "+" || y == "-")
		{
			if (!isInteger(NextToken()))
				throw syntaxError("integer expected after sign", getLine()); //process error: integer expetected after sign

			// Used kinda like sstream??
			y = y + token;
		}

		else if (y == "not")
		{
			int count = 1;

			while (NextToken() == "not")
			{
				++count;
			}

			if (isNonKeyID(token))
			{
				if (!inTable(token))
					throw semanticError("reference to undefined non-key identifier", getLine());

				y = find(symbolTable.begin(), symbolTable.end(), token)->value;
			}

			else
			{	
				if (!isBoolean(token))
					throw syntaxError("boolean expected after not", getLine());

				if (token == "true")
				{
					if (count % 2 == 1)
						y = "false";
					else
						y = "true";
				}
				else
				{
					if (count % 2 == 1)
						y = "true";
					else
						y = "false";
				}
			}
		}

		if (NextToken() != ";")
			throw syntaxError("semicolon expected", getLine()); // process error: semicolon expected
		
		Insert(x, WhichType(y), CONSTANT, WhichValue(y), YES, 1);

		if (NextToken() != "begin" && token != "var" && !isNonKeyID(token))
			throw syntaxError("non-keyword identifier, \"begin\", or \"var\" expected", getLine()); //process error: non-keyword identifier, "begin", or "var" expected

		// Need to check if var or if begin
		if (isNonKeyID(token))
				ConstStmts();
	} catch (baseException e) {
		throw;
	}
}

// Production 7
void Stage::VarStmts() //token should be NON_KEY_ID
{
	try {
		string x;
		storeType y;

		if (!isNonKeyID(token))
			throw syntaxError("non-keyword identifier expected", getLine()); //process error: non-keyword identifier expected

		x = Ids();

		if (token != ":")
			throw syntaxError("\":\" expected", getLine()); //process error: ":" expected

		if (NextToken() != "integer" && token != "boolean")
			throw syntaxError("illegal type follows \":\"", getLine()); // process error: illegal type follows ":"
		
		if (token == "integer")
			y = INTEGER;

		else
			y = BOOLEAN;

		if (NextToken() != ";")
			throw syntaxError("semicolon expected", getLine()); //process error: semicolon expected

		Insert(x, y, VARIABLE, "", YES, 1);

		if (NextToken() != "begin" && !isNonKeyID(token))
			throw syntaxError("non-keyword identifier or \"begin\" expected", getLine()); //process error: non-keyword identifier or "begin" expected

		if (isNonKeyID(token))
				VarStmts();

	} catch (baseException e) {
		throw;
	}
}

// Production 8
string Stage::Ids()  //token should be NON_KEY_ID
{
	string temp, tempString;

	if (!isNonKeyID(token))
		throw syntaxError("non-keyword identifier expected", getLine()); //process error: non-keyword identifier expected

	tempString = token;
	temp = token;

	if (NextToken() == ",")
	{
		if (!isNonKeyID(NextToken()))
			throw syntaxError("non-keyword identifier expected", getLine()); //process error: non-keyword identifier expected

		tempString = temp + "," + Ids();
	}

	return tempString;
}

// NonKeyID test
bool Stage::isNonKeyID(string token)
{
	int i = 1;

	// Check to see if token is actually a Keyword
	if (isKeyID(token))
		return false;

	// Make sure token is comprised only of letters, digits, and '_', and does not end with '_'
	if (isalpha(token[0]))
	{
		while (isalpha(token[i]) || isdigit(token[i]) || token[i] == '_')
			++i;

		return !(token[i - 1] == '_');
	}

	else
		return false;
}

bool Stage::isKeyID(string check)
{
	vector<string> keywords;

	keywords.push_back("program");
	keywords.push_back("const");
	keywords.push_back("var");
	keywords.push_back("integer");
	keywords.push_back("boolean");
	keywords.push_back("begin");
	keywords.push_back("end");
	keywords.push_back("true");
	keywords.push_back("false");
	keywords.push_back("not");
	keywords.push_back("mod");
	keywords.push_back("div");
	keywords.push_back("and");
	keywords.push_back("or");
	keywords.push_back("read");
	keywords.push_back("write");

	return (find(keywords.begin(), keywords.end(), check) != keywords.end());
}

bool Stage::isInteger(string token)
{
	istringstream testString(token.c_str());
	int test;

	// Try to put the value of testString into an integer variable
	testString >> test;

	// If testString is in a fail state, the above operation was no good
	if (!testString)
		return false;

	return true;
}

bool Stage::isLiteral(string name)
{
	vector<string> literals;
	literals.push_back("not");
	literals.push_back("+");
	literals.push_back("-");

	return (isInteger(name) || isBoolean(name) || find(literals.begin(), literals.end(), name) != literals.end());
}

bool Stage::isBoolean(string name)
{
	return (name == "true" || name == "false");
}

bool operator==(entry extName, string name)
{
	return extName.externalName == name;
}

bool operator==(entry extName, pair<storeType, string> lookFor)
{
	return (extName.dataType == lookFor.first && extName.value == lookFor.second);
}
