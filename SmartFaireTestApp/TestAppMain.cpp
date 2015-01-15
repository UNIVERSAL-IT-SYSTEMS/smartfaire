// Main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "arduino.h"
#include "ButtonPanel.h"
#include "LCDDisplay.h"
#include "spi.h"
#include "SpiMidi.h"

ButtonPanel buttonPanel;
LCDDisplay display;

int _tmain(int argc, _TCHAR* argv[])
{
    return RunArduinoSketch();
}

void setup()
{
	SPI.begin();

	display.Init();
	buttonPanel.Init();
	MusicShieldInit(true);

	display.ClearDisplay();
	display.WriteLine("Testing 1 2 3");
	display.WriteLine("12345678901234567890");
	display.WriteLine("ABCDEFGHIJKLMNOPQRST");
	display.WriteLine("abcdefghijklmnopqrst");
}

// the loop routine runs over and over again forever:
void loop()
{
	buttonPanel.WritePlayer1LEDs(buttonPanel.ReadPlayer1Buttons());
	buttonPanel.WritePlayer2LEDs(buttonPanel.ReadPlayer2Buttons());
	uint8_t b = buttonPanel.ReadPlayer1Buttons() | buttonPanel.ReadPlayer2Buttons();
	static uint8_t oldB = 0;
	if (b != oldB)
	{
		Sleep(50);	// Poor-man's debounce
		if (b & 1)
			noteOn(1, 50, 100);
		if (b & 2)
			noteOn(2, 52, 100);
		if (b & 4)
			noteOn(3, 53, 100);
		if (b & 8)
			noteOn(4, 55, 100);
		oldB = b;

	}
}