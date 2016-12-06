//Submitted Version
#include "stage2.h"

void Stage::CreateListingHeader()
{
	time_t now = time(0);
	
	listingFile << "STAGE1:\t\t" << "Michael McCarver - Eric Sunday\t\t" << ctime(&now) << endl;
	
	listingFile << "LINE NO." << setw(30) << "SOURCE STATEMENT" << endl << endl;
	

	/*
	print "STAGE0:", names, Date, TIME OF DAY;
	print "LINE NO:", "SOURCE STATEMENT";
	// line numbers and source statements should be aligned under the headings
	*/
}

void Stage::Parser()
{
	try {
		NextChar();
			  // charac must be initialized to the first character of the source file
		if (NextToken() != "program")
			throw syntaxError("keyword \"program\" expected", getLine());
				/*process error: keyword "program" expected;
				a call to NextToken() has two effects
					(1) the variable, token, is assigned the value of the next token
					(2) the next token is read from the source file in order to make
						the assignment.  The value returned by NextToken() is also
						the next token.*/

		Prog(); // Parser implements the grammar rules, calling first rule

	} catch (baseException e) {
		throw;
	}
}

void Stage::CreateListingTrailer()
{
	if (errorState)
	{
		listingFile << endl << "Error: Line " << errorLine << ": " << errorMsg << endl;
	}

	listingFile << endl << "COMPILATION TERMINATED" << setw(7) << errorNumber << " ERRORS ENCOUNTERED" << endl; 
}

void Stage::PrintSymbolTable()
{
	ostringstream newValue;

	vector<entry>::iterator it;

	if (!errorState)
	{
		for (it = symbolTable.begin(); it < symbolTable.end(); ++it)
		{
			if (it->alloc == YES)
			{
				if (it->mode == CONSTANT)
				{
					if (it->value == "true")
						Emit(it->internalName, "DEC", "0001", "", "", it->externalName);

					else if(it->value == "false")
						Emit(it->internalName, "DEC", "0000", "", "", it->externalName);

					else
					{
						if (it->value[0] == '-')
						{
							newValue << '-';
							it->value.erase(it->value.begin());
						}
						else
							newValue << '0';

						newValue << setw(3) << setfill('0') << it->value;
						Emit(it->internalName, "DEC", newValue.str(), "", "", it->externalName);
						newValue.str("");
					}
				}

				else
				{
					Emit(it->internalName, "BSS", "0001", "", "", it->externalName);
				}
			}
		}
		Code("end");
	}
}	

void Stage::newLine()
{
	++lineNumber;
	listingFile << setw(5) << lineNumber << "|";
}

void Stage::setErrorMessage(baseException &e)
{
	errorState = true;

	++errorNumber;

	errorMsg = e.what();

	errorLine = e.line();
}
