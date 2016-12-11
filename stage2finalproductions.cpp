#include "stage2.h"

void Stage::IfStmt()
{
	try {
		//'if' EXPRESS 'then' EXEC_STMT ELSE_PT

		string operand1, operand2, jump1;

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

		//jump2 = GetJump();

		ElsePt(jump1);

		//Code("NOP", jump1, "end if");

		

	} catch (baseException e) {
		throw;
	}
}

void Stage::ElsePt(string jump1)
{
	try {
		//'else' EXEC_STMT
		// epsilon

		string jump2;

		if (token == "else")
		{
			jump2 = GetJump();

			Code("UNJ", jump2, "jump to end if");
			Code("NOP", jump1, "else");

			if (NextToken() == "begin")
				BeginEndStmt();
			else
				ExecStmt();

			Code("NOP", jump2, "end if");
		}

		else
		{
			Code("NOP", jump1, "end if");
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

		string jump;

		if (token != "repeat")
			throw semanticError("keyword 'repeat' expected", getLine());

		jump = GetJump();

		Code("NOP", jump, "repeat");

		if (NextToken() == "begin")
			BeginEndStmt();
		else
			ExecStmts();

		if (token != "until")
			throw semanticError("keyword 'until' expected", getLine());

		Express();

		Code("AZJ", jump, "until");

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
