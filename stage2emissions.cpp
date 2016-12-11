#include "stage2.h"

void Stage::EmitProgramCode(string programName)
{
	string remark = programName;
	remark += " - Michael McCarver and Eric Sunday";

	Emit("STRT", "NOP", "", "", "", remark);
}

void Stage::EmitHltCode()
{
	Emit("", "HLT", "", "", "", "");
}

void Stage::EmitEndCode()
{
	// Emit last line of program
	Emit("", "END", "STRT", "", "", "");
}

void Stage::EmitReadCode(string operand1)
{
	string name, remark, strip = operand1;

	name = strip.substr(0, strip.find_first_of(","));
	strip.erase(0, strip.find_first_of(","));
	strip.erase(0, 1);

	while (name != "")
	{
		if (!inTable(name))
			throw semanticError("reference to undefined variable", getLine());
		
		if (find(symbolTable.begin(), symbolTable.end(), name)->mode == CONSTANT)
			throw semanticError("reading in of read-only location", getLine());

		remark = "read(";
		remark += name;
		remark += ")";

		Emit("", "RDI", getInternalName(name), "", "", remark);

		name = strip.substr(0, strip.find_first_of(","));
		strip.erase(0, strip.find_first_of(","));
		strip.erase(0, 1);
	}
}

void Stage::EmitWriteCode(string operand1)
{
	string name, remark, strip = operand1;

	name = strip.substr(0, strip.find_first_of(","));
	strip.erase(0, strip.find_first_of(","));
	strip.erase(0, 1);

	while (name != "")
	{
		if (!inTable(name))
			throw semanticError("reference to undefined variable", getLine());

		remark = "write(";
		remark += name;
		remark += ")";

		Emit("", "PRI", getInternalName(name), "", "", remark);

		name = strip.substr(0, strip.find_first_of(","));
		strip.erase(0, strip.find_first_of(","));
		strip.erase(0, 1);
	}
}

void Stage::EmitSubtractionCode(string operand1, string operand2)
{
	string inRegister = GetRegister(), 
			remark;

	remark = operand2;
	remark += " - ";
	remark += operand1;

    // If type of either operand is not integer -- process error
    if (!(WhichType(operand1) == INTEGER && WhichType(operand2) == INTEGER))
		throw syntaxError("both operands must be integers", getLine());

	if (inRegister != operand2)
	{
		// If A Register holds a temp not operand2
		if (isTemp(inRegister))
		{
			AssignRegister("DEASSIGN");
			Emit("", "STA", inRegister, "", "", "deassign AReg");
			ChangeAlloc(inRegister, YES);
			inRegister = "DEASSIGN";
		}

		// If A register holds a non-temp not operand1 -- deassign it
		else
		{
			// May not need this code written into .obj
			//Emit("", "STA", getInternalName(inRegister), "", "", "deassign AReg");
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		Emit("", "LDA", getInternalName(operand2), "", "", "");

		AssignRegister(operand2);
	}

	// Emit code to perform register-memory subtraction
	Emit("", "ISB", getInternalName(operand1), "", "", remark);

	// Deassign all temporaries involved in the subtraction -- free those names for reuse
	if (isTemp(operand1))
		FreeTemp();
	if (isTemp(operand2))
		FreeTemp();

	// ARegister = next available temporary name and change type to integer
	inRegister = GetTemp();
	AssignRegister(inRegister);
	ChangeStoreType(inRegister, INTEGER);

	// Push the name of the result onto operandStk
	PushOperand(inRegister);
}

void Stage::EmitAdditionCode(string operand1, string operand2)
{
	string inRegister = GetRegister(),
			remark;

	remark = operand2;
	remark += " + ";
	remark += operand1;

	// Throw error if operands aren't both INTEGER
	if (!(WhichType(operand1) == INTEGER && WhichType(operand2) == INTEGER))
		throw syntaxError("both operands must be integers", getLine());

	if (inRegister != operand1 && inRegister != operand2)
	{
		// If ARegister holds a temp not operand1 nor operand2
		if (isTemp(inRegister))
		{
			Emit("", "STA", inRegister, "", "", "deassign AReg");
			ChangeAlloc(inRegister, YES);
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// If ARegister holds a non-temp not operand1 nor operand2 -- deassign
		else
		{
			//Emit("", "STA", getInternalName(inRegister), "", "", "deassign AReg");
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// Since neither operand is in ARegister, load operand2
		Emit("", "LDA", getInternalName(operand2), "", "", "");
		AssignRegister(operand2);
		inRegister = operand2;
	}

	// Emit code to perform register-memory addition
	if (inRegister == operand2)
		Emit("", "IAD", getInternalName(operand1), "", "", remark);

	else
		Emit("", "IAD", getInternalName(operand2), "", "", remark);
	
	// Deassign all temporaries involved in the addition -- free those names for reuse
	if (isTemp(operand1))
		FreeTemp();
	if (isTemp(operand2))
		FreeTemp();
	
	// A Register = next available temporary name and change type to integer
	inRegister = GetTemp();
	AssignRegister(inRegister);
	ChangeStoreType(inRegister, INTEGER);

	// Push the name of the result onto operandStk
	PushOperand(inRegister);
}

void Stage::EmitNegationCode(string operand1)
{
	string inRegister = GetRegister(),
			remark;
	remark = "-";
	remark += operand1;

	if (!(WhichType(operand1) == INTEGER))
		throw syntaxError("operand must be an integer", getLine());

	if (isTemp(inRegister))
	{
		Emit("", "STA", inRegister, "", "", "deassign AReg");
		ChangeAlloc(inRegister, YES);
		AssignRegister("DEASSIGN");
		inRegister = "DEASSIGN";
	}

	else
	{
		//deassign code
		AssignRegister("DEASSIGN");
		inRegister = "DEASSIGN";
	}

	// Create ZERO constant for negation
	if (!inTable("ZERO"))
		Insert("ZERO", INTEGER, CONSTANT, "0", YES, 1);

	// Negation Code
	Emit("", "LDA", "ZERO", "", "", "");
	Emit("", "ISB", getInternalName(operand1), "", "", remark);

	// Deassign Temporaries involved
	if (isTemp(operand1))
		FreeTemp();

	// ARegister = next available temp name -- type INTEGER
	inRegister = GetTemp();
	AssignRegister(inRegister);
	ChangeStoreType(inRegister, INTEGER);

	// Push the name of the result onto operandStk
	PushOperand(inRegister);
}

void Stage::EmitNotCode(string operand1)
{
	string inRegister = GetRegister(),
			remark, jump = GetJump();

	remark = "not ";
	remark += operand1;

	if (!(WhichType(operand1) == BOOLEAN))
		throw syntaxError("operand must be boolean", getLine());

	if (inRegister != operand1)
	{
		// If AReg holds a temp not operand1
		if (isTemp(inRegister))
		{
			Emit("", "STA", inRegister, "", "", "deassign AReg");
			ChangeAlloc(inRegister, YES);
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// If AReg holds a non-temp not operand1 -- deassign
		else
		{
			//maybe emit code here
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// operand1 is not in ARegister
		Emit("", "LDA", getInternalName(operand1), "", "", "");
		AssignRegister(operand1);
		inRegister = operand1;
	}

	// Emit jump for boolean assignment
	Emit("", "AZJ", jump, "", "", remark);

	if (!inTable("FALSE"))
		Insert("FALSE", BOOLEAN, CONSTANT, "false", YES, 1);
	if (!inTable("TRUE"))
		Insert("TRUE", BOOLEAN, CONSTANT, "true", YES, 1);
	
	Emit("", "LDA", "FALS", "", "", "");
	Emit("", "UNJ", jump, "+", "1", "");
	Emit(jump, "LDA", "TRUE", "", "", "");

	if (isTemp(operand1))
		FreeTemp();

	// ARegister = next available temporary name and change type to boolean
	inRegister = GetTemp();
	AssignRegister(inRegister);
	ChangeStoreType(inRegister, BOOLEAN);

	// Push the name of the result onto operandStk
	PushOperand(inRegister);
}

void Stage::EmitDivisionCode(string operand1, string operand2)
{	
	string inRegister = GetRegister(),
			remark;

	remark = operand2;
	remark += " div ";
	remark += operand1;
	
	if (!(WhichType(operand1) == INTEGER && WhichType(operand2) == INTEGER))
		throw syntaxError("both operands must be integers", getLine());

	if (inRegister != operand2)
	{
		// If ARegister holds temp, deassign
		if (isTemp(inRegister))
		{
			Emit("", "STA", inRegister, "", "", "deassign AReg");
			ChangeAlloc(inRegister, YES);
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// If AReg holds a non-temp not operand1 nor operand2
		else
		{
			//emit??
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// load operand2
		Emit("", "LDA", getInternalName(operand2), "", "", "");
		AssignRegister(operand2);
		inRegister = operand2;
	}

	// Emit code to perform register-memory division
	Emit("", "IDV", getInternalName(operand1), "", "", remark);

	//deassign all temps involved in the  and free those names for reuse
	if(isTemp(operand1))
	     FreeTemp();
	if(isTemp(operand2))
	     FreeTemp();
	
	//A reg = next available temp name and change type of its symbol table entry to integer
	inRegister = GetTemp();
	AssignRegister(inRegister);
	ChangeStoreType(inRegister, INTEGER);
	
	//push the name of the result onto operandStk	
	PushOperand(inRegister); 
}

void Stage::EmitMultiplicationCode(string operand1, string operand2)
{
	string inRegister = GetRegister(),
			remark;

	remark = operand2;
	remark += " * ";
	remark += operand1;

	// Both types must be integers
	if (!(WhichType(operand1) == INTEGER && WhichType(operand2) == INTEGER))
		throw syntaxError("both operands must be integers", getLine());

	if (inRegister != operand1 && inRegister != operand2)
	{
		if (isTemp(inRegister))
		{
			Emit("", "STA", inRegister, "", "", "deassign AReg");
			ChangeAlloc(inRegister, YES);
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		else
		{
			// emit??
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// Load operand2
		Emit("", "LDA", getInternalName(operand2), "", "", "");
		AssignRegister(operand2);
		inRegister = operand2;
	}

	// Emit code to perform memory-register multiplication
	if (inRegister == operand2)
		Emit("", "IMU", getInternalName(operand1), "", "", remark);
	else
		Emit("", "IMU", getInternalName(operand2), "", "", remark);	

	// deassign all temps involved in the addition and free those names for reuse
	if(isTemp(operand1))
		FreeTemp();
	if(isTemp(operand2))
		FreeTemp();

	// AReg = next available temp name and change type of its symbol table entry to integer
	inRegister = GetTemp();
	AssignRegister(inRegister);
	ChangeStoreType(inRegister, INTEGER);

	// Push the name of the result onto operandStk	
	PushOperand(inRegister);
}

void Stage::EmitModuloCode(string operand1, string operand2)
{
	string inRegister = GetRegister(),
			remark;

	remark = operand2;
	remark += " mod ";
	remark += operand1;

    if (!(WhichType(operand1) == INTEGER && WhichType(operand2) == INTEGER))
		throw syntaxError("both operands must be integers", getLine());

	if (inRegister != operand2)
	{
		if (isTemp(inRegister))
		{
			Emit("", "STA", inRegister, "", "", "deassign AReg");
			ChangeAlloc(inRegister, YES);
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}
		{
			//emit?
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// Load operand2
		Emit("", "LDA", getInternalName(operand2), "", "", "");
		AssignRegister(operand2);
		inRegister = operand2;
	}

	// Emit code to perform memory-register modulus
	Emit("", "IDV", getInternalName(operand1), "", "", remark);

	// Deassign all temps involved
	if (isTemp(operand1))
		FreeTemp();
	if (isTemp(operand2))
		FreeTemp();

	// AReg = next availabe temp and change type
	inRegister = GetTemp();
	ChangeAlloc(inRegister, YES);
	AssignRegister(inRegister);
	ChangeStoreType(inRegister, INTEGER);

	// Move remainder into correct place
	Emit("", "STQ", inRegister, "", "", "store remainder in memory");
	Emit("", "LDA", inRegister, "", "", "load remainder from memory");

	// Push onto operandStk
	PushOperand(inRegister);
}

void Stage::EmitAndCode(string operand1, string operand2)
{
	string inRegister = GetRegister(),
			remark;

	remark = operand2;
	remark += " and ";
	remark += operand1;

	if (!(WhichType(operand1) == BOOLEAN && WhichType(operand2) == BOOLEAN))
		throw syntaxError("both operands must be boolean", getLine());

	if (inRegister != operand1 && inRegister != operand2)
	{
		// If temp in ARegister, deassign
		if (isTemp(inRegister))
		{
			Emit("", "STA", inRegister, "", "", "deassign AReg");
			ChangeAlloc(inRegister, YES);
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// If AReg holds a non temp not operand1 nor operand2 -- deassign
		else
		{
			//code maybe
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		Emit("", "LDA", getInternalName(operand2), "", "", "");
		AssignRegister(operand2);
		inRegister = operand2;
	}

	// Emit code to perform register-memory 'and' (multiplication)
	if (inRegister == operand2)
		Emit("", "IMU", getInternalName(operand1), "", "", remark);
	else
		Emit("", "IMU", getInternalName(operand2), "", "", remark);

	// Deassign temporaries involved
	if (isTemp(operand1))
		FreeTemp();
	if (isTemp(operand2))
		FreeTemp();

	// AReg = next available temp and change type to BOOLEAN
	inRegister = GetTemp();
	AssignRegister(inRegister);
	ChangeStoreType(inRegister, BOOLEAN);

	// Push operand onto stack
	PushOperand(inRegister);
}

void Stage::EmitOrCode(string operand1, string operand2)
{
	string inRegister = GetRegister(),
			remark, jump = GetJump();

	remark = operand2;
	remark += " or ";
	remark += operand1;

	if (!(WhichType(operand1) == BOOLEAN && WhichType(operand2) == BOOLEAN))
		throw syntaxError("both operands must be type boolean", getLine());

	if (inRegister != operand1 && inRegister != operand2)
	{
		// If temp, emit code to store and change allocation to yes
		if (isTemp(inRegister))
		{
			Emit("", "STA", inRegister, "", "", "deassign AReg");
			ChangeAlloc(inRegister, YES);
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// If non-temp, deassign
		else
		{
			//code?????????
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// Load operand2 into ARegister
		Emit("", "LDA", getInternalName(operand2), "", "", "");
		AssignRegister(operand2);
		inRegister = operand2;
	}

	// Insert TRUE
	if (!inTable("TRUE"))
		Insert("TRUE", BOOLEAN, CONSTANT, "true", YES, 1);

	// Emit code to perform memory-register 'or' (addition)
	if (inRegister == operand2)
		Emit("", "IAD", getInternalName(operand1), "", "", remark);
	else
		Emit("", "IAD", getInternalName(operand2), "", "", remark);

	Emit("", "AZJ", jump, "+", "1", "");
	Emit(jump, "LDA", "TRUE", "", "", "");

	// Free temporaries used in 'or'
	if (isTemp(operand1))
		FreeTemp();
	if (isTemp(operand2))
		FreeTemp();

	// AReg = next available temp and change type to boolean
	inRegister = GetTemp();
	AssignRegister(inRegister);
	ChangeStoreType(inRegister, BOOLEAN);

	// Push operand onto stack
	PushOperand(inRegister);
}

void Stage::EmitEqualsCode(string operand1, string operand2)
{
	string inRegister = GetRegister(),
			remark, jump = GetJump();

	remark = operand1;
	remark += " = ";
	remark += operand2;

	if (!(WhichType(operand1) == WhichType(operand2)))
		throw syntaxError("operands must be of same type", getLine());

	if (inRegister != operand1 && inRegister != operand2)
	{
		if (isTemp(inRegister))
		{
			// deassign ARegister
			Emit("", "STA", inRegister, "", "", "deassign AReg");
			ChangeAlloc(inRegister, YES);
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// If non-operand is in AReg -- deassign
		else
		{
			//code
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// Emit code to load operand2 into ARegister
		Emit("", "LDA", getInternalName(operand2), "", "", "");
		AssignRegister(operand2);
		inRegister = operand2;
	}

	// Emit code to perform register-memory '='
	if (inRegister == operand2)
		Emit("", "ISB", getInternalName(operand1), "", "", remark);
	else
		Emit("", "ISB", getInternalName(operand2), "", "", remark);

	// Insert FALS and TRUE
	if (!inTable("FALSE"))
		Insert("FALSE", BOOLEAN, CONSTANT, "false", YES, 1);
	if (!inTable("TRUE"))
		Insert("TRUE", BOOLEAN, CONSTANT, "true", YES, 1);

	// Jump for true/false assesment
	Emit("", "AZJ", jump, "", "", "");
	Emit("", "LDA", "FALS", "", "", "");
	Emit("", "UNJ", jump, "+", "1", "");
	Emit(jump, "LDA", "TRUE", "", "", "");

	// Free up temps used in '=' evaluation
	if (isTemp(operand1))
		FreeTemp();
	if (isTemp(operand2))
		FreeTemp();

	// AReg = next available temporary name and change type to boolean
	inRegister = GetTemp();
	AssignRegister(inRegister);
	ChangeStoreType(inRegister, BOOLEAN);

	// Push operand onto Stack
	PushOperand(inRegister);
}

void Stage::EmitAssignCode(string operand1, string operand2)
{
	string inRegister = GetRegister(),
			remark;

	remark = operand2;
	remark += " := ";
	remark += operand1;

	if (!(WhichType(operand1) == WhichType(operand2)))
		throw syntaxError("operands must be of same type", getLine());

	if (find(symbolTable.begin(), symbolTable.end(), operand1)->mode != VARIABLE)
		throw syntaxError("identifier to the let of ':=' must be a variable", getLine());

	if (inRegister != operand1)
	{
		// Temporary won't be in AReg, but non-temp may be -- deassign
		//code
		AssignRegister("DEASSIGN");
		inRegister = "DEASSIGN";

		// Load operand1 into AReg
		Emit("", "LDA", getInternalName(operand1), "", "", "");
		AssignRegister(operand1);
		inRegister = operand1;
	}

	// Emit code for register-memory ':='
	Emit("", "STA", getInternalName(operand2), "", "", remark);

	// Free Temps
	if (isTemp(operand1))
		FreeTemp();
	if (isTemp(operand2))
		FreeTemp();

	AssignRegister(operand2);
	inRegister = operand2;
}

void Stage::EmitGreaterThanCode(string operand1, string operand2)
{
	string inRegister = GetRegister(), jump = GetJump(),
			remark;

	remark = operand2;
	remark += " > ";
	remark += operand1;
	
	if (!(WhichType(operand1) == INTEGER && WhichType(operand2) == INTEGER))
		throw syntaxError("both operands must be integers", getLine());

	if (inRegister != operand2)
	{
		if (isTemp(inRegister))
		{
			// Deassign
			Emit("", "STA", inRegister, "", "", "deassign AReg");
			ChangeAlloc(inRegister, YES);
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// If non-temp, deassign
		else
		{
			//code
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// Load operand2 into ARegister
		Emit("", "LDA", getInternalName(operand2), "", "", "");
		AssignRegister(operand2);
		inRegister = operand2;
	}

	// Insert FALS/TRUE into symbol table
	if (!inTable("TRUE"))
		Insert("TRUE", BOOLEAN, CONSTANT, "true", YES, 1);
	if (!inTable("FALSE"))
		Insert("FALSE", BOOLEAN, CONSTANT, "false", YES, 1);

	// Emit code to perform register-memory '>'
	Emit("", "ISB", getInternalName(operand1), "", "", remark);
	Emit("", "AMJ", jump, "", "", "");
	Emit("", "AZJ", jump, "", "", "");
	Emit("", "LDA", "TRUE", "", "", "");
	Emit("", "UNJ", jump, "+", "1", "");
	Emit(jump, "LDA", "FALS", "", "", "");

	// Free temporaries involved
	if (isTemp(operand1))
		FreeTemp();
	if (isTemp(operand2))
		FreeTemp();

	// AReg = next available temp - change type to boolean
	inRegister = GetTemp();
	AssignRegister(inRegister);
	ChangeStoreType(inRegister, BOOLEAN);

	// Push operand onto Stack
	PushOperand(inRegister);
}

void Stage::EmitLessThanCode(string operand1, string operand2)
{
	string inRegister = GetRegister(), jump = GetJump(),
			remark;

	remark = operand2;
	remark += " < ";
	remark += operand1;

	if (!(WhichType(operand1) == INTEGER && WhichType(operand2) == INTEGER))
		throw syntaxError("both operands must be boolean", getLine());

	if (inRegister != operand2)
	{
		if (isTemp(inRegister))
		{
			// Deassign
			Emit("", "STA", inRegister, "", "", "deassign AReg");
			ChangeAlloc(inRegister, YES);
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// If non-temp - deassign
		else
		{
			//code
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// Load operand2 into ARegister
		Emit("", "LDA", getInternalName(operand2), "", "", "");
		AssignRegister(operand2);
		inRegister = operand2;
	}

	// Insert True/False into symbol table
	if (!inTable("FALSE"))
		Insert("FALSE", BOOLEAN, CONSTANT, "false", YES, 1);
	if (!inTable("TRUE"))
		Insert("TRUE", BOOLEAN, CONSTANT, "true", YES, 1);

	// Emit code to perform register-memory '<'
	Emit("", "ISB", getInternalName(operand1), "", "", remark);
	Emit("", "AMJ", jump, "", "", "");
	Emit("", "LDA", "FALS", "", "", "");
	Emit("", "UNJ", jump, "+", "1", "");
	Emit(jump, "LDA", "TRUE", "", "", "");

	// Free up temporaries
	if (isTemp(operand1))
		FreeTemp();
	if (isTemp(operand2))
		FreeTemp();

	// AReg = next available temp and change type to boolean
	inRegister = GetTemp();
	AssignRegister(inRegister);
	ChangeStoreType(inRegister, BOOLEAN);

	// Push operand onto stack
	PushOperand(inRegister);
}

void Stage::EmitGreaterEqualCode(string operand1, string operand2)
{
	string inRegister = GetRegister(), jump = GetJump(),
			remark;

	remark = operand2;
	remark += " >= ";
	remark += operand1;

	if (!(WhichType(operand1) == INTEGER && WhichType(operand2) == INTEGER))
		throw syntaxError("both operands must be integers", getLine());

	 if (inRegister != operand2)
	 {
	 	if (isTemp(inRegister))
	 	{
	 		Emit("", "STA", inRegister, "", "", "deassign AReg");
	 		ChangeAlloc(inRegister, YES);
	 		AssignRegister("DEASSIGN");
	 		inRegister = "DEASSIGN";
	 	}

	 	// if nontemp - deassign
	 	else
	 	{
	 		//code
	 		AssignRegister("DEASSIGN");
	 		inRegister = "DEASSIGN";
	 	}

	 	// Load operand2 into AReg
	 	Emit("", "LDA", getInternalName(operand2), "", "", "");
	 	AssignRegister(operand2);
	 	inRegister = operand2;
	 }

	 // Inser True/False into symbol table
	if (!inTable("TRUE"))
		Insert("TRUE", BOOLEAN, CONSTANT, "true", YES, 1);
	if (!inTable("FALSE"))
		Insert("FALSE", BOOLEAN, CONSTANT, "false", YES, 1);

	// Emit code to perform register-memory '>='
	Emit("", "ISB", getInternalName(operand1), "", "", remark);
	Emit("", "AMJ", jump, "", "", "");
	Emit("", "LDA", "TRUE", "", "", "");
	Emit("", "UNJ", jump, "+", "1", "");
	Emit(jump, "LDA", "FALS", "", "", "");

	// Deassign temporaries involved
	if (isTemp(operand1))
		FreeTemp();
	if(isTemp(operand2))
		FreeTemp();

	// ARegister = next available temp and change type to boolean
	inRegister = GetTemp();
	AssignRegister(inRegister);
	ChangeStoreType(inRegister, BOOLEAN);

	// Push operand onto stack
	PushOperand(inRegister);
}

void Stage::EmitLessEqualCode(string operand1, string operand2)
{
	string inRegister = GetRegister(), jump = GetJump(),
			remark;

	remark = operand2;
	remark += " <= ";
	remark += operand1;

	if (!(WhichType(operand1) == INTEGER && WhichType(operand2) == INTEGER))
		throw syntaxError("both operands must be integers", getLine());

	if (inRegister != operand2)
	{
		if (isTemp(inRegister))
		{
			Emit("", "STA", inRegister, "", "", "deassign AReg");
			ChangeAlloc(inRegister, YES);
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		else
		{
			//code
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// Load operand2 into ARegister
		Emit("", "LDA", getInternalName(operand2), "", "", "");
		AssignRegister(operand2);
		inRegister = operand2;
	}

	// Insert True/False into symbolTable
	if (!inTable("FALSE"))
		Insert("FALSE", BOOLEAN, CONSTANT, "false", YES, 1);
	if (!inTable("TRUE"))
		Insert("TRUE", BOOLEAN, CONSTANT, "true", YES, 1);

	// Emit code for register-memory '<='
	Emit("", "ISB", getInternalName(operand1), "", "", remark);
	Emit("", "AMJ", jump, "", "", "");
	Emit("", "AZJ", jump, "", "", "");
	Emit("", "LDA", "FALS", "", "", "");
	Emit("", "UNJ", jump, "+", "1", "");
	Emit(jump, "LDA", "TRUE", "", "", "");

	// Free temporaries involved
	if (isTemp(operand1))
		FreeTemp();
	if (isTemp(operand2))
		FreeTemp();

	// ARegister = next available temporary and change type to boolean
	inRegister = GetTemp();
	AssignRegister(inRegister);
	ChangeStoreType(inRegister, BOOLEAN);

	// Push operand onto stack
	PushOperand(inRegister);
}

void Stage::EmitNotEqualCode(string operand1, string operand2)
{
	string inRegister = GetRegister(), jump = GetJump(),
			remark;

	remark = operand1;
	remark += " <> ";
	remark += operand2;

	if (!(WhichType(operand1) == WhichType(operand2)))
		throw syntaxError("operands must be same data type", getLine());

	if (inRegister != operand2)
	{
		if (isTemp(inRegister))
		{
			Emit("", "STA", inRegister, "", "", "deassign AReg");
			ChangeAlloc(inRegister, YES);
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		else
		{
			//code
			AssignRegister("DEASSIGN");
			inRegister = "DEASSIGN";
		}

		// Load operand2 into ARegister
		Emit("", "LDA", getInternalName(operand2), "", "", "");
		AssignRegister(operand2);
		inRegister = operand2;
	}

	 // Inser True into symbol table
	if (!inTable("TRUE"))
		Insert("TRUE", BOOLEAN, CONSTANT, "true", YES, 1);

	// Emit code to perform register-memory '<>'
	Emit("", "ISB", getInternalName(operand1), "", "", remark);
	Emit("", "AZJ", jump, "+", "1", "");
	Emit(jump, "LDA", "TRUE", "", "", "");

	// Free temporaries used
	if (isTemp(operand1))
		FreeTemp();
	if (isTemp(operand2))
		FreeTemp();

	// ARegister = next available temp and change type to boolean
	inRegister = GetTemp();
	AssignRegister(inRegister);
	ChangeStoreType(inRegister, BOOLEAN);

	// Push operand onto stack
	PushOperand(inRegister);
}

// This code will print the RAMMcode into .obj
void Stage::Emit(string location, string opcode, string address, string sign,
	string number, string remarks)
{
	objectFile << left;

	objectFile << setw(4) << location;

	objectFile << "  ";

	objectFile << setw(3) << opcode;

	objectFile << " ";

	objectFile << setw(4) << address;

	objectFile << setw(1) << sign;

	objectFile << setw(1) << number;

	objectFile << "   ";

	objectFile << remarks;

	objectFile << endl << right;
}
