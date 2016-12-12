#include "stage2.h"

void Stage::IfStmt()
{
	try {
		//'if' EXPRESS 'then' EXEC_STMT ELSE_PT

		string jump1, operand1;

		if (token != "if")
			throw semanticError("keyword 'if' expected", getLine());

		NextToken();

		Express();

		if (!operandStk.empty())
		{
			operand1 = PopOperand();

			if (!isTemp(operand1))
			{
				if (isBoolean(operand1))
				{
					if (!inTable(operand1))
						Insert(operand1, BOOLEAN, CONSTANT, operand1, YES, 1);
				}
				else if (WhichType(operand1) != BOOLEAN)
					throw semanticError("predicate must be boolean valued", getLine());

				if (GetRegister() != operand1)
					Code("LOAD", getInternalName(operand1));
			}
		}

		if (isTemp(GetRegister()))
			FreeTemp();
		AssignRegister("DEASSIGN");

		jump1 = GetJump();

		Code("AZJ", jump1, "if false jump to " + jump1);

		if (token != "then")
			throw semanticError("keyword 'then' expected", getLine());

		if (NextToken() == "begin")
		{
			endStk.push('a');
			BeginEndStmt();

			if (NextToken() == "end")
			{
				tricky = true;
			}
		}
		else
		{
			ExecStmt();

			NextToken();
		}

		ElsePt(jump1);

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
			{
				endStk.push('a');
				BeginEndStmt();
			}

			else
				ExecStmt();

			Code("NOP", jump2, "end if");
		}

		else
		{
			Code("NOP", jump1, "end if");

			if (!tricky)
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

		string jump1 = GetJump(), jump2, operand1;

		if (token != "while")
			throw syntaxError("keyword 'while' expected", getLine());

		NextToken();

		Code("NOP", jump1, "while");

		Express();

		if (!operandStk.empty())
		{
			operand1 = PopOperand();

			if (!isTemp(operand1))
			{
				if (isBoolean(operand1))
				{
					if (!inTable(operand1))
						Insert(operand1, BOOLEAN, CONSTANT, operand1, YES, 1);
				}

				else if (WhichType(operand1) != BOOLEAN)
					throw semanticError("predicate must be boolean valued", getLine());

				Code("LOAD", getInternalName(operand1));
			}
		}

		if (token != "do")
			throw syntaxError("keyword 'do' expected", getLine());

		jump2 = GetJump();

		Code("AZJ", jump2, "do");

		if (isTemp(GetRegister()))
			FreeTemp();
		AssignRegister("DEASSIGN");


		if (NextToken() == "begin")
		{
			endStk.push('a');
			BeginEndStmt();
		}
		else
			ExecStmt();

		AssignRegister("DEASSIGN");

		Code("UNJ", jump1, "end while");

		Code("NOP", jump2);

	} catch (baseException e) {
		throw;
	}
}

void Stage::RepeatStmt()
{
	try {
		//'repeat' EXEC_STMTS 'until' EXPRESS

		string jump, operand1;

		if (token != "repeat")
			throw semanticError("keyword 'repeat' expected", getLine());

		jump = GetJump();

		Code("NOP", jump, "repeat");

		if (isTemp(GetRegister()))
		FreeTemp();

		AssignRegister("DEASSIGN");

		NextToken();

		while (token != "until")
		{
			ExecStmt();

			NextToken();
		}

		if (token != "until")
			throw semanticError("keyword 'until' expected", getLine());

		NextToken();

		Express();

		if (!operandStk.empty())
		{
			operand1 = PopOperand();

			if (!isTemp(operand1))
			{
				if (isBoolean(operand1))
				{
					if (!inTable(operand1))
						Insert(operand1, BOOLEAN, CONSTANT, operand1, YES, 1);
				}
				else if (WhichType(operand1) != BOOLEAN)
					throw semanticError("predicate must be boolean valued", getLine());

				if (GetRegister() != operand1)
					Code("LOAD", getInternalName(operand1));
			}
		}

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

		//NextToken();

	} catch (baseException e) {
		throw;
	}
}
