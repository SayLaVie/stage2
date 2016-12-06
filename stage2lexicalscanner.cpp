//Submitted Version
#include "stage2.h"

string Stage::NextToken() //returns the next token or end of file marker
{
	// Vector for checking for special characters
	vector<char> specialchars;
	specialchars.push_back('=');
	specialchars.push_back(':');
	specialchars.push_back(',');
	specialchars.push_back(';');
	specialchars.push_back('.');
	specialchars.push_back('+');
	specialchars.push_back('-');
	specialchars.push_back('*');
	specialchars.push_back('<');
	specialchars.push_back('>');
	specialchars.push_back('(');
	specialchars.push_back(')');

	token = "";

	while (token == "")
	{
		if (charac == '{')
		{
			while (NextChar() != '}' && charac != END_OF_FILE); // Spin loop to process comments

			if (charac == END_OF_FILE)
				throw lexicalError("unexpected end of file", getLine()); // Process error: unexpected end of file

			NextChar();
		}

		else if (charac == '}')
			throw lexicalError("'}' cannot begin token", getLine()); // Process error: '}' cannot begin token

		else if (isspace(charac))
			NextChar();

		else if (find(specialchars.begin(), specialchars.end(), charac) != specialchars.end())
		{
			token = charac;
			NextChar();

			if (token == ":" && charac == '=')
			{
				token += charac;
				NextChar();
			}

			else if (token == "<" && (charac == '>' || charac == '='))
			{
				token += charac;
				NextChar();
			}

			else if (token == ">" && charac == '=')
			{
				token += charac;
				NextChar();
			}
		}

		else if (isupper(charac))
			throw lexicalError("token cannot begin with uppercase letter", getLine()); // Process error: Token cannot begin with uppercase letter

		else if (isalpha(charac))
		{
			token = charac;

			while (isalpha(NextChar()) || isdigit(charac) || charac == '_')
				token += charac;

			if (charac == '_')
				throw lexicalError("'_' cannot end token", getLine()); // Process error: '_' cannot end token

			token = token.substr(0, 15);			
		}

		else if (isdigit(charac))
		{
			token = charac;

			while (isdigit(NextChar()))
				token += charac;
		}

		else if (charac == END_OF_FILE)
			token = charac;

		else
			throw lexicalError("illegal symbol", getLine()); // Process error: illegal symbol
	}
	
	return token;
}

char Stage::NextChar() //returns the next character or end of file marker
{
	sourceFile.get(charac);

	if (!sourceFile.eof())
	{
		if (lastChar == '\n')
			newLine();
		// print to listing file (starting new line if necessary)
		listingFile << charac;
	}

	else
		charac = END_OF_FILE;
	
	lastChar = charac;

	return charac;
}

// void Stage::PrintChar(char ch)
// {

// }
