#pragma once

#include "mcp23017.h"

class LCDDisplay
{
public:
	LCDDisplay();
	~LCDDisplay();

	void Init();
	void ClearDisplay();
	void MoveToLine(unsigned char line);
	void WriteLine(char *format, ...);
	bool GetButton1();
	bool GetButton2();

private:
	MCP23017 _expander = MCP23017(0x21);
	unsigned char _line;

	void ResetSequence();
	void Send4BitInstruction(unsigned char d);
	void Send8BitCharacter(unsigned char d);
	void Send4Bits(unsigned char control, unsigned char d);
	void SetDisplayAddress(unsigned char addr);
	unsigned char FlipAndShift(unsigned char src);

};

