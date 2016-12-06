Stage 2 -- final stage of compiler project

Files from earlier stages:


----Stage0----

stage0.h

	Class decleration of Stages and function declerations

stage0main.cpp

	Main function implementing the Stages class

stage0productions.cpp

	Prog()
	ProgStmt()
	Consts()
	Vars()
	BeginEndStmt()
	ConstStmts()
	VarStmts()
	Ids()

stage0actionroutines.cpp

	Insert()
	WhichType(string)
	WhichValue(string)

stage0lexicalscanner.cpp

	NextToken()
	NextChar()

stage0frommain.cpp

	CreateListingHeader()
	Parser()
	CreateListingTrailer()
	PrintSymbolTable()



----Stage1----

stage1newproductions.cpp

	ExecStmts()
	ExecStmt()
	AssignStmt()
	ReadStmt()
	ReadList()
	WriteStmt()
	WriteList()
	Express()
	Expresses()
	Term()
	Terms()
	Factor()
	Factors()
	Part()
	RelOp()
	AddLevelOp()
	MultLevelOp()

stage1translationgrammaractions.cpp

	Code(operator, operand1, operand2)
	PushOperator(operator)
	PopOperator()
	PushOperand(operand)
	PopOperand()

stage1emissions.cpp

	EmitAdditionCode()
	EmitDivisionCode()
	EmitMultiplicationCode()
	EmitAndCode()
	EmitOrCode()
	EmitEqualCode()
	EmitAssignCode()

stage1tempmanip.cpp

	FreeTemp()
	GetTemp()
	AssignRegister()3


----New Files for Stage2----

unsure for now..