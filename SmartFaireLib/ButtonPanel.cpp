#include "ButtonPanel.h"

// Buttons for player 1.
const unsigned char Player1Buttons = 0xf0;

// Buttons for player 2.
const unsigned char Player2Buttons = 0x0f;

// Number of bits to shift when reading player 1
const unsigned char Player1Shift = 4;

// Number of bits to shift when reading player 2
const unsigned char Player2Shift = 0;

ButtonPanel::ButtonPanel()
{
	_leds = 0;
}

ButtonPanel::~ButtonPanel()
{
}

void ButtonPanel::Init()
{
	_expander.Init();

	// Configure port A for input
	_expander.SetPortAInvert(Player1Buttons | Player2Buttons);
	_expander.SetPortAPullUp(Player1Buttons | Player2Buttons);
	_expander.SetPortAInputPins(Player1Buttons | Player2Buttons);

	// Configure port B for output
	_expander.SetPortBInputPins(0x00);
	_expander.WritePortB(0x00);
}

unsigned char ButtonPanel::ReadPlayer1Buttons()
{
	return (_expander.ReadPortA() & Player1Buttons) >> Player1Shift;
}

unsigned char ButtonPanel::ReadPlayer2Buttons()
{
	return (_expander.ReadPortA() & Player2Buttons) >> Player2Shift;
}

void ButtonPanel::WritePlayer1LEDs(unsigned char leds)
{
	// clear player 1 bits and leave player 2 bits alone
	_leds &= Player2Buttons;
	// Turn on our bits and write
	_leds |= (leds << Player1Shift);
	_expander.WritePortB(_leds);
}

void ButtonPanel::WritePlayer2LEDs(unsigned char leds)
{
	// clear player 2 bits and leave player 1 bits alone
	_leds &= Player1Buttons;
	// Turn on our bits and write
	_leds |= (leds << Player2Shift);
	_expander.WritePortB(_leds);
}

