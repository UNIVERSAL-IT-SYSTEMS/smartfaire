#pragma once

#include "wire.h"

// Pin IDs
const unsigned char GPA0 = 0x01;
const unsigned char GPA1 = 0x02;
const unsigned char GPA2 = 0x04;
const unsigned char GPA3 = 0x08;
const unsigned char GPA4 = 0x10;
const unsigned char GPA5 = 0x20;
const unsigned char GPA6 = 0x40;
const unsigned char GPA7 = 0x80;

const unsigned char GPB0 = 0x01;
const unsigned char GPB1 = 0x02;
const unsigned char GPB2 = 0x04;
const unsigned char GPB3 = 0x08;
const unsigned char GPB4 = 0x10;
const unsigned char GPB5 = 0x20;
const unsigned char GPB6 = 0x40;
const unsigned char GPB7 = 0x80;

class MCP23017
{
public:
	MCP23017(unsigned char address);
	virtual ~MCP23017();

	void Init();

	void SetPortAInputPins(unsigned char pin);
	void SetPortBInputPins(unsigned char pin);

	void SetPortAInvert(unsigned char pin);
	void SetPortBInvert(unsigned char pin);

	void SetPortAPullUp(unsigned char pin);
	void SetPortBPullUp(unsigned char pin);

	unsigned char ReadPortA();
	unsigned char ReadPortB();

	void WritePortA(unsigned char pinToSet);
	void WritePortB(unsigned char pinToSet);

private:
	unsigned char _address;
	TwoWire _i2c;
};

