//Submitted Version

#include "stage2.h"

//testing input
// Multiply inserted names are illegal
void Stage::Insert(string externalName,storeType inType, modes inMode, string inValue,
    allocation inAlloc, int inUnits)
{
    string name, strip = externalName;
    entry insert;  // Set values for the entry insert
    insert.dataType = inType;
    insert.mode = inMode;
    insert.value = inValue;
    insert.alloc = inAlloc;
    insert.units = inUnits;

    name = strip.substr(0, strip.find_first_of(","));
    strip.erase(0, strip.find_first_of(","));
    strip.erase(0, 1);

    while (name != "")
    {
    	if (find(symbolTable.begin(), symbolTable.end(), name) != symbolTable.end())
            throw semanticError("multiple name definition", getLine()); // process error: multiple name definition

        else if (isKeyID(name))
            throw semanticError("illegal use of keyword", getLine());  // process error: illegal use of keyword

        else if (symbolTable.size() == 256)
            throw semanticError("symbol table overflow", getLine());  // Too many entries in the symbol table

        else  // Create Table Entry
        {
            if (isupper(name[0]))
            {
                insert.externalName = name;
                insert.internalName = name.substr(0, 4);
                symbolTable.push_back(insert);
            }
            else
            {
            	insert.externalName = name;
            	insert.internalName = GenInternalName(inType);
            	symbolTable.push_back(insert);
            }
        }

        name = strip.substr(0, strip.find_first_of(","));
        strip.erase(0, strip.find_first_of(","));
        strip.erase(0, 1);
    }
}

storeType Stage::WhichType(string name) //tells which data type a name has
{
	storeType dType;
	vector<entry>::iterator it;

	if (isLiteral(name))
	{
		if (isBoolean(name))
			dType = BOOLEAN;

		else
			dType = INTEGER;
	}

	else // name is an identifier and hopefully a constant
	{
        it = find(symbolTable.begin(), symbolTable.end(), name);
		if (it != symbolTable.end())
            dType = it->dataType;

        else
            throw semanticError("reference to undefined variable", getLine());  // Process error: reference to undefined constant

	}

	return dType;
}

string Stage::WhichValue(string name) //tells which value a name has
{
	string value;
	vector<entry>::iterator it;

    if (isLiteral(name)) // If name is a literal
        value = name;

    else  // Name is an identifier and hopefully a constant
    {
        it = find(symbolTable.begin(), symbolTable.end(), name);
    	if (it != symbolTable.end())
    		value = it->value;
    	else
    		throw semanticError("reference to undefined variable", getLine());// Process error: reference to undefined constant
    }

	return value;
}

string Stage::GenInternalName(storeType type)
{
	ostringstream name;

	name.str("");

	if (type == PROG_NAME)
		name << "P0";

	else if (type == INTEGER)
		name << "I" << internalGenNum[0]++;

	else
		name << "B" << internalGenNum[1]++;

	return name.str();
}
