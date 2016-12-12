#include "stage2.h"

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

void Stage::EmitLoadCode(string loader)
{
	Emit("", "LDA", loader, "", "", "");
}