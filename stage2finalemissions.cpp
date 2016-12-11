#include "stage2.h"

void Stage::EmitIf()
{
}

void Stage::EmitElse()
{
}

void Stage::EmitNOPCode(string jump, string remark)
{
	Emit(jump, "NOP", "", "", "", remark);
}

void Stage::EmitAZJCode(string jump, string remark)
{
	Emit("", "AZJ", jump, "", "", remark);
}

void Stage::EmitUNJCode(string jump, string remark)
{
	Emit("", "UNJ", jump, "", "", remark);
}

/*void Stage::EmitWhile(string operand1, string operand2, string oper)
{
	string jump1 = GetJump(), jump2;

	Emit(jump1, "NOP", "", "", "", "while");

	Code(oper, operand1, operand2);

	if (token != "do")
		throw semanticError("expected keyword 'do'", getLine());

	NextToken();

	jump2 = GetJump();

	Emit("","AZJ", jump2, "", "", "do");

	ExecStmt();

	Emit("", "UNJ", jump1, "", "", "end while");

	Emit(jump2, "NOP", "", "", "", "");
}*/

void Stage::Repeat()
{
}