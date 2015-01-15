// Main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "arduino.h"
#include "ButtonPanel.h"
#include "LCDDisplay.h"

ButtonPanel panel;
LCDDisplay display;

void PrintLonelyText();
void DoLonelyModeLoop();
bool GameIsRunning();
void WaitForGameToExit();
bool OnePlayerButtonPressed();
bool TwoPlayerButtonPressed();
void LaunchOnePlayerGame();
void LaunchTwoPlayerGame();

const WCHAR* OnePlayerApp = L"c:\\test\\Follow.exe";
const WCHAR* TwoPlayerApp = L"c:\\test\\Speed.exe";

HANDLE GameProcess = NULL;

int _tmain(int argc, _TCHAR* argv[])
{
    return RunArduinoSketch();
}

void setup()
{
	panel.Init();
	display.Init();

	PrintLonelyText();
}

// the loop routine runs over and over again forever:
void loop()
{
	if (GameIsRunning())
	{
		WaitForGameToExit();
		PrintLonelyText();
	}
	else
	{
		DoLonelyModeLoop();
	}
}

void PrintLonelyText()
{
	display.ClearDisplay();
	display.WriteLine("Welcome to Makerfair");
	display.WriteLine("--------------------");
	display.WriteLine("Press any button");
	display.WriteLine("to begin a game");
}

unsigned char GetNextLight(unsigned char mask)
{
	mask <<= 1;
	if (mask >= 0x10)
	{
		mask = 0x01;
	}
	return mask;
}

void DoLonelyModeLoop()
{
	static const unsigned int cycleTime = 100;
	static unsigned char lightMask = 0x08;
	static unsigned int lastChange = 0;

	if (millis() - lastChange > cycleTime)
	{
		lightMask = GetNextLight(lightMask);

		panel.WritePlayer1LEDs(lightMask);
		panel.WritePlayer2LEDs(lightMask);

		lastChange = millis();
	}

	if (OnePlayerButtonPressed())
	{
		LaunchOnePlayerGame();
	}
	else if (TwoPlayerButtonPressed())
	{
		LaunchTwoPlayerGame();
	}
}

bool GameIsRunning()
{
	// FUTURE: it would be nice to check if GameProcess is really our process and not a recycled handle
	return ((GameProcess != NULL) && WaitForSingleObject(GameProcess, 0) == WAIT_TIMEOUT);
}

void WaitForGameToExit()
{
	// FUTURE: it would be nice to check if GameProcess is really our process and not a recycled handle
	WaitForSingleObject(GameProcess, INFINITE);
}

bool OnePlayerButtonPressed()
{
	return ((panel.ReadPlayer1Buttons() != 0) || display.GetButton1());
}

bool TwoPlayerButtonPressed()
{
	return ((panel.ReadPlayer2Buttons() != 0) || display.GetButton2());
}

void LaunchGame(const WCHAR *AppName)
{
	// FUTURE: This should check for an existing process before launching
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFO si = { 0 };
	if (CreateProcess(AppName, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		GameProcess = pi.hProcess;
	}
	else
	{
		GameProcess = NULL;
	}
}

void LaunchOnePlayerGame()
{
	LaunchGame(OnePlayerApp);
}

void LaunchTwoPlayerGame()
{
	LaunchGame(TwoPlayerApp);
}

