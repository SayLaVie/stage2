// Stage 2 Compiler
// Michael McCarver
// Eric Sunday

#include "error.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <stack>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <utility>

using namespace std;

/*const int MAX_SYMBOL_TABLE_SIZE = 256;
enum storeType {INTEGER, BOOLEAN, PROG_NAME, UNKNOWN};
enum allocation {YES, NO};
enum modes {VARIABLE, CONSTANT};*/

/*struct entry	// define symbol table entry format
{
	string internalName;
	string externalName;
	storeType dataType;
	modes mode;
	string value;
	allocation alloc;
	int units;
};*/

class Stage
{
private:
	static const int MAX_SYMBOL_TABLE_SIZE = 256;
	enum storeType {INTEGER, BOOLEAN, PROG_NAME, UNKNOWN};
	enum allocation {YES, NO};
	enum modes {VARIABLE, CONSTANT};

	struct entry	// define symbol table entry format
	{
		string internalName;
		string externalName;
		storeType dataType;
		modes mode;
		string value;
		allocation alloc;
		int units;
	};


	int *internalGenNum;
	vector<entry> symbolTable;
	//vector<entry>::iterator symTableIterator;
	ifstream sourceFile;
	ofstream listingFile, objectFile;
	string token;
	char charac, lastChar;
	int lineNumber, currentTempNo, currentJumpNo, maxTempNo;
	stack<string> operandStk, operatorStk;
	string inRegA;
	static string errorMsg;
	static int errorLine, errorNumber;
	static bool errorState;
	static const char END_OF_FILE = '$';


	// stage2productions.cpp
	void Prog();
	void ProgStmt();
	void Consts();
	void Vars();
	void BeginEndStmt();
	void ConstStmts();
	void VarStmts();
	string Ids();
	bool isNonKeyID(string);
	bool isKeyID(string);
	bool isInteger(string);	

	// stage2newproductions.cpp
	void ExecStmts();
	void ExecStmt();
	void AssignStmt();
	void ReadStmt();
	void WriteStmt();
	void Express();
	void Term();
	void Expresses();
	void Factor();
	void Terms();
	void Part();
	void Factors();
	void ReadList();
	void WriteList();
	string RelOp(string);
	string AddLevelOp(string);
	string MultLevelOp(string);
	bool isRelOp(string);
	bool isAddLevelOp(string);
	bool isMultLevelOp(string);

	// stage2emissions.cpp
	void EmitProgramCode(string);
	void EmitEndCode();
	void EmitHltCode();
	void EmitReadCode(string);
	void EmitWriteCode(string);
	void EmitSubtractionCode(string, string);
	void EmitAdditionCode(string, string);
	void EmitNegationCode(string);
	void EmitNotCode(string);
	void EmitDivisionCode(string, string);
	void EmitMultiplicationCode(string, string);
	void EmitModuloCode(string, string);
	void EmitAndCode(string, string);
	void EmitOrCode(string, string);
	void EmitEqualsCode(string, string);
	void EmitAssignCode(string, string);
	void EmitGreaterThanCode(string, string);
	void EmitLessThanCode(string, string);
	void EmitGreaterEqualCode(string, string);
	void EmitLessEqualCode(string, string);
	void EmitNotEqualCode(string, string);
	void Emit(string, string, string, string, string, string);

	// stage2tempmanip
	void FreeTemp();
	string GetTemp();
	string GetJump();
	void AssignRegister(string);
	string GetRegister();
	bool isTemp(string);
	void ChangeAlloc(string, allocation);
	void ChangeStoreType(string, storeType);
	string getInternalName(string);
	bool inTable(string);
	//vector<entry>::iterator tableLocate(string);

	//stage2translationgrammaractions.cpp
	void Code(string, string = "", string = "");
	void PushOperator(string);
	void PushOperand(string);
	string PopOperator();
	string PopOperand();

	// stage2actionroutines.cpp
	void Insert(string, storeType, modes, string, allocation , int);
	storeType WhichType(string name);
	string WhichValue(string name);
	string GenInternalName(storeType);
	bool isLiteral(string);
	bool isBoolean(string);

	// stage2lexicalscanner.cpp
	string NextToken();
	char NextChar();

	// stage2frommain.cpp
	void newLine();

public:
	// Constructor
	Stage(char *ifile, char *oflist, char *ofobj)
	{
		lastChar = '\n';
		internalGenNum = new int[2]; // Index 0 is INTEGER, 1 is BOOLEAN
		internalGenNum[1] = internalGenNum[0] = 0;
		lineNumber = 0;
		errorNumber = 0;
		currentTempNo = -1;
		currentJumpNo = -1;
		maxTempNo = -1;
		inRegA = "DEASSIGN";

		sourceFile.open(ifile);
		listingFile.open(oflist);
		objectFile.open(ofobj);

		if (!sourceFile.good())
			throw fileOpenError("Problem opening source file");
		if (!listingFile.good())
			throw fileOpenError("Problem opening listing file");
		if (!objectFile.good())
			throw fileOpenError("Problem opening object file");
	}
	~Stage()
	{
		delete[] internalGenNum;
		sourceFile.close();
		listingFile.close();
		objectFile.close();
	}

	int getLine()
	{
		return lineNumber;
	}

	// stage2frommain.cpp
	void CreateListingHeader();
	void Parser();
	void CreateListingTrailer();
	void PrintSymbolTable();
	void setErrorMessage(baseException&);

	friend bool operator==(entry extName, string name)
	{
		return extName.externalName == name;
	}

	friend bool operator==(entry extName, pair<storeType, string> lookFor)
	{
		return (extName.dataType == lookFor.first && extName.value == lookFor.second);
	}
};
