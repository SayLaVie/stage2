//Submitted Version

#ifndef ERROR_H
#define ERROR_H

#include <string>

using namespace std;

// Base of our exception class -- our exceptions are derived from this
class baseException
{
public:
	baseException(const string& msg = "", const string& id = "") : msgString(msg), errorType(id)
	{
		if (msgString == "")
			msgString = "Unspecified exception";
		if (id == "")
			errorType = "Unspecified exception";
	}

	baseException(const string& msg, const string& id, const int& line) : msgString(msg), errorType(id)
	{
		msgString = msg;
		errorType = id;
		lineNumber = line;
	}

	string what() const
	{
		return msgString;
	}

	string type() const
	{
		return errorType;
	}

	int line() const
	{
		return lineNumber;
	}

	void operator=(const baseException &e)
	{
		msgString = e.what();

		errorType = e.type();

		lineNumber = e.line();
	}

protected:
	string msgString;
	string errorType;
	int lineNumber;
};


// Lexical Errors -- tests if tokens are legal in any possible context; NextToken()
// is the lexical scanner from Stage 0
class lexicalError : public baseException
{
public:
	/*lexicalError(const string& msg = "") : baseException(msg, "Lexical Error")
	{}*/
	lexicalError(const string& msg, const int& lineNum) : baseException(msg, "Lexical Error", lineNum)
	{}
};

// Syntactic Errors -- violation of the grammatical rules and structural patterns
// governing the ordered use of tokens in a particular programming language
class syntaxError : public baseException
{
public:
	/*syntaxError(const string& msg = "") : baseException(msg, "Syntax Error")
	{}*/
	syntaxError(const string& msg, const int& lineNum) : baseException(msg, "Syntax Error", lineNum)
	{}
};

// Semantic -- meaning; detected by action routines; Insert() from stage 0
class semanticError : public baseException
{
public:
	/*semanticError(const string& msg = "") : baseException(msg, "Semantic Error")
	{}*/
	semanticError(const string& msg, const int& lineNum) : baseException(msg, "Semantic Error", lineNum)
	{}
};

// Compiler -- error within the compiler itself
class compilerError : public baseException
{
public:
	/*compilerError(const string& msg = "") : baseException(msg, "Compiler Error")
	{}*/
	compilerError(const string& msg, const int& lineNum) : baseException(msg, "Compiler Error", lineNum)
	{}
};

class fileOpenError : public baseException
{
public:
	fileOpenError(const string& msg = "") : baseException(msg, "File Error")
	{}
};

#endif
