#include "stage2.h"

void Stage::IfStmt()
{
	try {
		//'if' EXPRESS 'then' EXEC_STMT ELSE_PT

		string operand1, operand2, jump1, jump2;

		if (token != "if")
			throw semanticError("keyword 'if' expected", getLine());

		NextToken();

		Express();

		operand1 = PopOperand();
		operand2 = PopOperand();

		Code(PopOperator(), operand1, operand2);

		jump1 = GetJump();

		Code("AZJ", jump1, "if false jump to " + jump1);

		if (token != "then")
			throw semanticError("keyword 'then' expected", getLine());

		if (NextToken() == "begin")
			BeginEndStmt();
		else
			ExecStmt();

		ElsePt();

		

	} catch (baseException e) {
		throw;
	}
}

void Stage::ElsePt
{
	try {
		//'else' EXEC_STMT
		// epsilon

		if (token == "else")
		{
			ExecStmt();
		}

	} catch (baseException e) {
		throw;
	}
}

void Stage::WhileStmt()
{
	try {
		//'while' EXPRESS 'do' EXEC_STMTS

		string jump1 = GetJump(), jump2, operand1, operand2;

		if (token != "while")
			throw syntaxError("keyword 'while' expected", getLine());

		NextToken();

		Express();

		Code("NOP", jump1, "while");

		operand1 = PopOperand();
		operand2 = PopOperand();

		Code(PopOperator(), operand1, operand2);

		if (NextToken() != "do")
			throw syntaxError("keyword 'do' expected", getLine());

		jump2 = GetJump();

		Code("AZJ", jump2, "do");

		if (token == "begin")
			BeginEndStmt();
		else
			ExecStmt();

		Code("UNJ", jump1, "end while");

		Code("NOP", jump2);

		NextToken();

	} catch (baseException e) {
		throw;
	}
}

void Stage::RepeatStmt()
{
	try {
		//'repeat' EXEC_STMTS 'until' EXPRESS

		if (token != "repeat")
			throw semanticError("keyword 'repeat' expected", getLine());

		ExecStmts();

		if (token != "until")
			throw semanticError("keyword 'until' expected", getLine());

		Express();

	} catch (baseException e) {
		throw;
	}
}

void Stage::NullStmt()
{
	try {
		//';'

		if (token != ";")
			throw semanticError("';' expected", getLine());

		NextToken();

	} catch (baseException e) {
		throw;
	}
}
