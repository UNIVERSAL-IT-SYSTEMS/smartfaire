#pragma once
#include "mcp23017.h"

class ButtonPanel
{
public:
	ButtonPanel();
	~ButtonPanel();

	void Init();

	unsigned char ReadPlayer1Buttons();
	unsigned char ReadPlayer2Buttons();

	void WritePlayer1LEDs(unsigned char leds);
	void WritePlayer2LEDs(unsigned char leds); 

private:
	unsigned char _leds;
	MCP23017 _expander = MCP23017(0x20);

};

