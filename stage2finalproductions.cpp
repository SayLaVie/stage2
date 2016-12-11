#include "stage2.h"

void Stage::IfStmt()
{
	//'if' EXPRESS 'then' EXEC_STMT ELSE_PT

	if (token != "if")
		throw semanticError("keyword 'if' expected", getLine());

	Express();

	if (token != "then")
		throw semanticError("keyword 'then' expected", getLine());

	ExecStmt();

	// NextToken()??

	ElsePt();
}

void Stage::ElsePt
{
	//'else' EXEC_STMT
	// epsilon

	if (token == "else")
	{
		ExecStmt();
	}
}

void Stage::WhileStmt()
{
	//'while' EXPRESS 'do' EXEC_STMT

	if (token != "while")
		throw semanticError("expected keyword 'while'", getLine());

	Express();

	if (token != "do")
		throw semanticError("expected keyword 'do'", getLine());

	ExecStmt();
}

void Stage::RepeatStmt()
{
	//'repeat' EXEC_STMTS 'until' EXPRESS

	if (token != "repeat")
		throw semanticError("keyword 'repeat' expected", getLine());

	ExecStmts();

	if (token != "until")
		throw semanticError("keyword 'until' expected", getLine());

	Express();
}

void Stage::NullStmt()
{
	//';'

	if (token != ";")
		throw semanticError("';' expected", getLine());

	NextToken();
}