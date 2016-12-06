#include "stage2.h"

void Stage::Code(string oper, string operand1, string operand2)
{
	try {
		if (oper == "program")
			EmitProgramCode(operand1);

		else if (oper == "hlt")
			EmitHltCode();
			
		else if (oper == "end")
			EmitEndCode();
			
		else if (oper == "read")
			EmitReadCode(operand1);
			
		else if (oper == "write")
			EmitWriteCode(operand1);
			
		else if (oper == "+")
			EmitAdditionCode(operand1, operand2);
			
		else if (oper == "-")
			EmitSubtractionCode(operand1, operand2);
			
		else if (oper == "neg")
			EmitNegationCode(operand1);
			
		else if (oper == "not")
			EmitNotCode(operand1);
			
		else if (oper == "*")
			EmitMultiplicationCode(operand1, operand2);
			
		else if (oper == "div")
			EmitDivisionCode(operand1, operand2);
			
		else if (oper == "mod")
			EmitModuloCode(operand1, operand2);
			
		else if (oper == "and")
			EmitAndCode(operand1, operand2);
			
		else if (oper == "or")
			EmitOrCode(operand1, operand2);
						
		else if (oper == "=")
			EmitEqualsCode(operand1, operand2);
			
		else if (oper == ">")
			EmitGreaterThanCode(operand1, operand2);
			
		else if (oper == ">=")
			EmitGreaterEqualCode(operand1, operand2);
			
		else if (oper == "<")
			EmitLessThanCode(operand1, operand2);
			
		else if (oper == "<=")
			EmitLessEqualCode(operand1, operand2);
			
		else if (oper == "<>")
			EmitNotEqualCode(operand1, operand2);
			
		else if (oper == ":=")
			EmitAssignCode(operand1, operand2);
			
		else //process error: undefined operation
			throw syntaxError("undefined operation", getLine());

	} catch (baseException e) {
		throw;
	}
}

void Stage::PushOperator(string name) //push name onto operatorStk
{
	operatorStk.push(name);
}

void Stage::PushOperand(string name) //push name onto operandStk. If name is literal, also create a symbol table entry for it
{
	if (isInteger(name))
	{
		if (!inTable(name)) // may change to something looking for same value in symbolTable
		{
			Insert(name, INTEGER, CONSTANT, WhichValue(name), YES, 1);
		}

		operandStk.push(name);
	}

	else if (isBoolean(name))
	{
		if (!inTable(name))
		{
			entry insert;
			vector<entry>::iterator it = find(symbolTable.begin(), symbolTable.end(), make_pair(WhichType(name), WhichValue(name)));

			if (it != symbolTable.end())
				operandStk.push(it->externalName);

			else
			{
				if (symbolTable.size() == 256)
					throw semanticError("symbol table overflow", getLine());

				insert.externalName = name;
				insert.internalName = GenInternalName(BOOLEAN);
				insert.dataType = BOOLEAN;
				insert.mode = CONSTANT;
				insert.value = name;
				insert.alloc = YES;
				insert.units = 1;
				symbolTable.push_back(insert);

				operandStk.push(name);
			}
		}

		else
			operandStk.push(name);
	}

	else
	{
		operandStk.push(name);
	}
}

string Stage::PopOperator() //pop name from operatorStk
{
	string oper;

	if (!operatorStk.empty())
	{
		oper = operatorStk.top();
		operatorStk.pop();
		return oper;
	}

	else
		throw compilerError("underflow error", getLine());
}

string Stage::PopOperand() //pop name from operandStk
{
	string operand;

	if (!operandStk.empty())
	{
		operand = operandStk.top();
		operandStk.pop();
		return operand;
	}
		
	else
		throw compilerError("underflow error", getLine());
}
