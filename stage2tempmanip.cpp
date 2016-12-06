/* Functions are: 
FreeTemp()
GetTemp()
AssignRegister()*/

#include "stage2.h"

void Stage::FreeTemp()
{
	currentTempNo--;
	if(currentTempNo < -1)
	{
		throw compilerError("currentTempNo should be >= -1", getLine());
	}
}


string Stage::GetTemp()
{
	ostringstream temp;

	currentTempNo++;

	temp << "T" << currentTempNo;

	if(currentTempNo > maxTempNo)
	{
		Insert(temp.str(), UNKNOWN, VARIABLE, "", NO, 0);

		maxTempNo++;
	}
	
	return temp.str();
	
}

string Stage::GetJump()
{
	ostringstream jump;

	currentJumpNo++;

	jump << "L" << currentJumpNo;

	return jump.str();
}

void Stage::AssignRegister(string reg)
{
	inRegA = reg;
}

string Stage::GetRegister()
{
	return inRegA;
}

bool Stage::isTemp(string reg)
{
	return reg[0] == 'T';
}

void Stage::ChangeAlloc(string temp, allocation alloc)
{
	vector<entry>::iterator symTableIterator;

	symTableIterator = find(symbolTable.begin(), symbolTable.end(), temp);

	symTableIterator->alloc = alloc;
	
	symTableIterator->units = 1;
}

void Stage::ChangeStoreType(string temp, storeType type)
{
	vector<entry>::iterator symTableIterator;

	symTableIterator = find(symbolTable.begin(), symbolTable.end(), temp);

	symTableIterator->dataType = type;
}

string Stage::getInternalName(string who)
{
	vector<entry>::iterator symTableIterator;

	symTableIterator = find(symbolTable.begin(), symbolTable.end(), who);

	return symTableIterator->internalName;
}

bool Stage::inTable(string who)
{
	return (find(symbolTable.begin(), symbolTable.end(), who) != symbolTable.end());
}
