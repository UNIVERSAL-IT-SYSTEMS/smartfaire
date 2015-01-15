// Main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "arduino.h"
#include "ButtonPanel.h"
#include "LCDDisplay.h"

ButtonPanel panel;
LCDDisplay display;

void PlayTheGame();

int _tmain(int argc, _TCHAR* argv[])
{
	return RunArduinoSketch();
}

void setup()
{
	panel.Init();
	display.Init();
}

// the loop routine runs over and over again forever:
void loop()
{
	PlayTheGame();
}


void PrintSpeedInstructions()
{
	display.ClearDisplay();
	display.WriteLine("Today we are playing");
	display.WriteLine("       Speed");
	display.WriteLine("Press the lit button");
	display.WriteLine("as fast as you can!");
}

void PrintGameOverTwoPlayer(unsigned int p1, unsigned int p2)
{
	if (p1 > p2)
	{
		display.WriteLine("Player 1 wins");
	}
	else if (p2 > p1)
	{
		display.WriteLine("Player 2 wins");
	}
	else
	{
		display.WriteLine("Its a tie");
	}
	display.WriteLine("Player 1 - %d pts", p1);
	display.WriteLine("Player 2 - %d pts", p2);
}

void PrintGameOverOnePlayer(unsigned int p)
{
	display.WriteLine("You got %d points", p);
}

void PrintGameOver(unsigned int p1, unsigned int p2)
{
	display.ClearDisplay();
	display.WriteLine("Game Over!");
	if (p1 > 0 && p2 > 0)
	{
		PrintGameOverTwoPlayer(p1, p2);
	}
	else if (p1 > 0)
	{
		PrintGameOverOnePlayer(p1);
	}
	else if (p2 > 0)
	{
		PrintGameOverOnePlayer(p2);
	}
}

void FlashWinner(unsigned int p1, unsigned int p2)
{
	static unsigned int totalFlashTime = 5000;
	static unsigned int flashInterval = 250;
	unsigned int flashStart = millis();
	while (millis() - flashStart <= totalFlashTime)
	{
		panel.WritePlayer1LEDs(0);
		panel.WritePlayer2LEDs(0);
		Sleep(flashInterval);

		if (p1 >= p2)
		{
			panel.WritePlayer1LEDs(0xf);
		}
		if (p2 >= p1)
		{
			panel.WritePlayer2LEDs(0xf);
		}
		Sleep(flashInterval);
	}

}

void PrintTimeOut()
{
	display.ClearDisplay();
	display.WriteLine("For more info");
	display.WriteLine("Check Out the");
	display.WriteLine("Microsoft booth at");
	display.WriteLine("this event.");
}

unsigned char GetRandomButtonMask(unsigned char oldMask)
{
	// Only provide options that have 1 or 2 lights lit.  
	static unsigned char validMasks[] = {
		b0001,
		b0010,
		b0011,
		b0100,
		b0101,
		b0110,
		b1000,
		b1001,
		b1010,
		b1100,
	};
	while (1)
	{
		unsigned char newMask = validMasks[random(_countof(validMasks))];
		if (newMask != oldMask)
			return newMask;
	}
}

unsigned int p1Count = 0;
unsigned int p2Count = 0;
unsigned char p1Mask = 0;
unsigned char p2Mask = 0;

void GameLoop()
{
	// Get new masks if we need them.
	if (p1Mask == 0)
	{
		p1Mask = GetRandomButtonMask(p1Mask);
		panel.WritePlayer1LEDs(p1Mask);
	}
	if (p2Mask == 0)
	{
		p2Mask = GetRandomButtonMask(p2Mask);
		panel.WritePlayer2LEDs(p2Mask);
	}

	// Check to see if we have hits.  If so, up the count and 
	// null the mask so we get a new one the next time around.
	if (panel.ReadPlayer1Buttons() == p1Mask)
	{
		p1Count++;
		p1Mask = 0;
	}
	if (panel.ReadPlayer2Buttons() == p2Mask)
	{
		p2Count++;
		p2Mask = 0;
	}

}

void PlayTheGame()
{
	static const unsigned int gameTime = 20000;
	unsigned int gameStart = millis();
	PrintSpeedInstructions();

	p1Count = 0;
	p2Count = 0;
	p1Mask = 0;
	p2Mask = 0;

	while (millis() - gameStart <= gameTime)
	{
		GameLoop();
	}

	PrintGameOver(p1Count, p2Count);
	FlashWinner(p1Count, p2Count);
	throw new _arduino_quit_exception();
}


