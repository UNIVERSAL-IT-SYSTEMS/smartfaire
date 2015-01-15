#include "LCDDisplay.h"
#include "binary.h"
#include "arduino.h"

const unsigned char ENABLE_BIT = 1 << 5;
const unsigned char READ_BIT = 1 << 6;
const unsigned char DATA_BIT = 1 << 7;

const unsigned char WRITE_INSTRUCTION = 0x0;
const unsigned char WRITE_DATA = DATA_BIT;

const unsigned char BUTTON_1 = 0x01;
const unsigned char BUTTON_2 = 0x02;
const unsigned char BUTTONS = BUTTON_1 | BUTTON_2;

LCDDisplay::LCDDisplay()
{
	_line = 0;
}

LCDDisplay::~LCDDisplay()
{
}

void LCDDisplay::Init()
{
	_expander.Init();
	_expander.SetPortBInputPins(0x00);

	_expander.SetPortAInputPins(BUTTONS);
	_expander.SetPortAPullUp(BUTTONS);
	_expander.SetPortAInvert(BUTTONS);

	ResetSequence();

}

void LCDDisplay::ResetSequence()
{
	// Send Reset function to init 4-bit interface
	// Table 12 from HD44780U spec

	Send4BitInstruction(b0011);
	Sleep(4);
	Send4BitInstruction(b0011);
	Send4BitInstruction(b0011);

	// Function set (Set interface to be 4 bits long.) Interface is 8 bits in length.
	Send4BitInstruction(b0010);

	// Set number of lines and font
	Send4BitInstruction(b0010);
	Send4BitInstruction(b1000);	// NF**

	// Turn on display.  No cursor
	Send4BitInstruction(b0000);
	Send4BitInstruction(b1100);

	// Entry mode set - increment addr by 1, shift cursor by right.
	Send4BitInstruction(b0000);
	Send4BitInstruction(b0110);

	ClearDisplay();
}

void LCDDisplay::Send4BitInstruction(unsigned char c)
{
	Send4Bits(WRITE_INSTRUCTION, c);
}

void LCDDisplay::Send8BitCharacter(unsigned char c)
{
	Send4Bits(WRITE_DATA, c >> 4);
	Send4Bits(WRITE_DATA, c & 0xf);
}

// The AdaFruit LCD display has the 4 bits shifted by 1 and flipped.
unsigned char LCDDisplay::FlipAndShift(unsigned char src)
{
	unsigned char dest = 0;
	if ((src & 0x1) != 0)
	{
		dest |= 0x10;
	}
	if ((src & 0x2) != 0)
	{
		dest |= 0x08;
	}
	if ((src & 0x4) != 0)
	{
		dest |= 0x04;
	}
	if ((src & 0x8) != 0)
	{
		dest |= 0x02;
	}
	return dest;
}

void LCDDisplay::Send4Bits(unsigned char control, unsigned char d)
{
	unsigned char portB = (control | FlipAndShift(d) | ENABLE_BIT);
	_expander.WritePortB(portB);
	portB &= ~ENABLE_BIT;
	_expander.WritePortB(portB);
}

void LCDDisplay::ClearDisplay()
{
	// Clear display
	Send4BitInstruction(b0000);
	Send4BitInstruction(b0001);

	// return to zero
	Send4BitInstruction(b0000);
	Send4BitInstruction(b0010);

	_line = 0;
}

void LCDDisplay::SetDisplayAddress(unsigned char addr)
{
	Send4BitInstruction(0x08 | (addr >> 4));
	Send4BitInstruction(addr & 0x0f);
}

void LCDDisplay::WriteLine(char *format, ...)
{
	va_list args;
	int len = 0;
	char *buffer = NULL;

	va_start(args, format);
	len = _vscprintf(format, args) + 1;
	buffer = new char[len];
	if (buffer != NULL)
	{
		vsprintf_s(buffer, len, format, args);
	}

	MoveToLine(_line);
	_line++;
	_line %= 4;

	char *p = buffer;
	while (*p)
	{
		Send8BitCharacter(*p);
		p++;
	}

	delete[](buffer);

}

void LCDDisplay::MoveToLine(unsigned char line)
{
	_line = line;
	switch (line % 4)
	{
	case 0:
		SetDisplayAddress(0);
		break;
	case 1:
		SetDisplayAddress(64);
		break;
	case 2:
		SetDisplayAddress(20);
		break;
	case 3:
		SetDisplayAddress(84);
		break;
	}

}

bool LCDDisplay::GetButton1()
{
	return (_expander.ReadPortA() & BUTTON_1) ? true : false;
}

bool LCDDisplay::GetButton2()
{
	return (_expander.ReadPortA() & BUTTON_2) ? true : false;
}



