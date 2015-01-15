// Main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "arduino.h"
#include "ButtonPanel.h"
#include "LCDDisplay.h"
#include "spimidi.h"
#include "mp3.h"

int main(int argc, _TCHAR* argv[])
{
	return RunArduinoSketch();
}

// Number of players
const int NumberOfPlayers = 2;

// Number of buttons per side
const uint8_t ButtonsPerSide = 4;

// Maximum number of rounds presses per game
const int MaxRoundsPerGame = 100;

// Array of buttons for the entire game.
uint8_t Buttons[MaxRoundsPerGame];

// Current progress - number of buttons matched so far
int CurrentRound = 0;

// Delay between buttons
const unsigned long DelayBetweenButtons = 500;

// How long does the user have to press a key?
const unsigned long ButtonPressTimeout = 5000;

// Cheer sound
const char *CheerSound = "c:\\test\\cheer.mp3";

// buzzer sound
const char *BuzzerSound = "c:\\test\\buzzer.mp3";

// well known value for "no button pressed"
const uint8_t ButtonNotPressed = -1;

// well known value for "multiple buttons pressed"
const uint8_t MultipleButtonsPressed = -2;

// Messages for a good game
char* CongratsMessage[] = {
	"congratulations!",
	"woo hoo!",
	"kudos!",
	"well done!",
	"very good!",
	"cheers!",
	"good work!",
	"mazel tov!",
	"awesome!",
	"fantastic!",
	"great!"
	"superb!",
	"super!"
	"wonderful!",
	"amazing!",
	"not bad!",
	"impressive!",
	"cool!",
	"WOW!",
	"fancy shootin', Tex!",
	"good job!",
	"Top notch!",
	"Far out!"
}; 


// Enum with possible states for each player
enum PlayState {
	PlayState_promptingForNextRound,
	PlayState_waitingForButtonDown,
	PlayState_waitingForButtonUp,
	PlayState_waitingForOtherPlayer,
	PlayState_notPlaying,
	PlayState_doneBecauseOfTimeOut,
	PlayState_doneBecauseOfBadEntry
};

// Structure with per-player play information
struct PlayerStruct
{
	PlayState		State;
	int				PressCountForThisRound;
	uint8_t			ButtonIndex;
	unsigned long	TimeStartedWaiting;
};

// Our two player instances
PlayerStruct P[NumberOfPlayers];

// Objects for underlying hardware
ButtonPanel buttonPanel;
LCDDisplay display; 

// Convert a state to the string name for debug output
char *StateToName(PlayState s)
{
#define StateToString(s) case(s): return #s;
	switch (s)
	{
		StateToString(PlayState_promptingForNextRound);
		StateToString(PlayState_waitingForButtonDown);
		StateToString(PlayState_waitingForButtonUp);
		StateToString(PlayState_waitingForOtherPlayer);
		StateToString(PlayState_notPlaying);
		StateToString(PlayState_doneBecauseOfTimeOut);
		StateToString(PlayState_doneBecauseOfBadEntry);
		default: return "unkonwn";
	}
}

// Return the given players state as a string
char *StateString(int p)
{
	return StateToName(P[p].State);
}

// Print instructions for this game
void PrintFollowMeInstructions()
{
	display.ClearDisplay();
	display.WriteLine("Today we are playing");
	display.WriteLine("     Follow Me");
	display.WriteLine("Press the buttons in");
	display.WriteLine("   the same order");
}

// Initialize the player structure for the given player
void InitPlayerStruct(int Player)
{
	P[Player].State = PlayState_promptingForNextRound;
	P[Player].ButtonIndex = ButtonNotPressed;
	P[Player].PressCountForThisRound = 0;
	P[Player].TimeStartedWaiting = 0;
}

// Initiate conditions for a new game
void StartNewGame()
{
	Log("Starting new game\n");
	PrintFollowMeInstructions();
	for(int i = 0; i < _countof(Buttons); i++)
	{
		Buttons[i] = (uint8_t)random(ButtonsPerSide);
	}
	CurrentRound = 0;
	for (int p = 0; p < NumberOfPlayers; p++)
	{
		InitPlayerStruct(p);
	}
}

// Play the button sound for one of the players
void PlayButtonSound(int Player, uint8_t Button)
{
	int channel = Player + 1;
	switch (Button)
	{
	case 0:
		noteOn(channel, 50, 100);
		Sleep(250);
		break;
	case 1:
		noteOn(channel, 52, 100);
		Sleep(250);
		break;
	case 2:
		noteOn(channel, 53, 100);
		Sleep(250);
		break;
	case 3:
		noteOn(channel, 55, 100);
		Sleep(250);
		break;
	}
}

// Set the LED for the given player
void WriteLED(int Player, uint8_t LED)
{
	if (Player == 0)
		buttonPanel.WritePlayer1LEDs(LED);
	else
		buttonPanel.WritePlayer2LEDs(LED);
}

// Read the button for the given player
uint8_t ReadButton(int Player)
{
	if (Player == 0)
		return buttonPanel.ReadPlayer1Buttons();
	else
		return buttonPanel.ReadPlayer2Buttons();
}

// Is the player still playing?
bool PlayerIsStillPlaying(int Player)
{
	return ((P[Player].State != PlayState_doneBecauseOfTimeOut) && 
		(P[Player].State != PlayState_doneBecauseOfBadEntry) &&
		(P[Player].State != PlayState_notPlaying));
}

// Is the other player still playing?
bool OtherPlayerIsStillPlaying(int Player)
{
	return PlayerIsStillPlaying(Player == 0 ? 1 : 0);
}


// Play the next sequence for the user
void PlayNextSequence()
{
	Log("%s: P0=%s, P0=%s\n", __FUNCTION__, StateString(0), StateString(1));
	Sleep(DelayBetweenButtons);
	for (int i = 0; i <= CurrentRound; i++)
	{
		for (int p = 0; p < NumberOfPlayers; p++)
		{
			if (PlayerIsStillPlaying(p))
			{
				WriteLED(p, 1 << Buttons[i]);
			}
		}

		PlayButtonSound(0, Buttons[i]);

		for (int p = 0; p < NumberOfPlayers; p++)
		{
			WriteLED(p, 0);
		}

		Sleep(DelayBetweenButtons);
	}
}

// Play an MP3 file.
void PlayMp3(const char *FileName)
{
	MP3 mp3;

	mp3.Init();
	mp3.PlayFileSync(FileName);
	MusicShieldInit(true);	// Switch back to MIDI playback
}

// Get input for the given player and convert the button mask to a button index
uint8_t GetInput(int Player)
{
	uint8_t buttons = ReadButton(Player);
	if (buttons != 0)
	{
		// Poor-man's debounce
		Sleep(50);
		// Act on it.
		switch (buttons)
		{
		case 1:
			return 0;
		case 2:
			return 1;
		case 4:
			return 2;
		case 8:
			return 3;
		default:
			return MultipleButtonsPressed;
		}
	}
	return ButtonNotPressed;
}

// Tell the player that they've won
void HandleState_DonePlaying()
{
	display.ClearDisplay();
	display.WriteLine("Game Over!");
	display.WriteLine("You got %d presses", CurrentRound);
	if (CurrentRound >= 5)
	{
		display.WriteLine(CongratsMessage[CurrentRound % _countof(CongratsMessage)]);
		PlayMp3(CheerSound);
	}
	Sleep(5000);
	throw new _arduino_quit_exception;
	StartNewGame();
}

// Setup method.  Called once when app starts
void setup()
{
	display.Init();
	buttonPanel.Init();
	MusicShieldInit(true);
	StartNewGame();
}

// Move the given player to a button wait state
void MoveToButtonWaitingState(int Player)
{
	if (P[Player].State == PlayState_promptingForNextRound)
	{
		P[Player].TimeStartedWaiting = millis();
		P[Player].State = PlayState_waitingForButtonDown;
		P[Player].PressCountForThisRound = 0;
	}
}

// Handle the prompting for next round state
void HandleState_PromptingForNextRound()
{
	if (P[0].State == PlayState_promptingForNextRound || P[1].State == PlayState_promptingForNextRound)
	{
		Log("%s: P0=%s, P0=%s\n", __FUNCTION__, StateString(0), StateString(1));
		PlayNextSequence();
		MoveToButtonWaitingState(0);
		MoveToButtonWaitingState(1);
	}
}

// Handle the end of round condition
void HandleState_EndOfRound()
{
	bool moveToNextRound = false;
	if (P[0].State == PlayState_waitingForOtherPlayer)
	{
		if (!PlayerIsStillPlaying(1))
		{
			// 0 is waiting for 1, but 1 isn't playing.  On to the next round.
			P[0].State = PlayState_promptingForNextRound;
			moveToNextRound = true;
		}
		else if (P[1].State == PlayState_waitingForOtherPlayer)
		{
			// 0 is waiting for 1 and 1 is waiting for 0.  On to the next round.
			P[0].State = PlayState_promptingForNextRound;
			P[1].State = PlayState_promptingForNextRound;
			moveToNextRound = true;
		}
	}
	else if (P[1].State == PlayState_waitingForOtherPlayer)
	{
		if (!OtherPlayerIsStillPlaying(1))
		{
			// 1 is waiting for 0, but 0 isn't playing.  On to the next round.
			P[1].State = PlayState_promptingForNextRound;
			moveToNextRound = true;
		}
	}
	if (moveToNextRound)
	{
		Log("%s: P0=%s, P0=%s\n", __FUNCTION__, StateString(0), StateString(1));
		CurrentRound++;
		// TODO: handle case where CurrentRound > 100
	}
	// Else, no transition to make.  
}

// Handle a button down event
void HandleEvent_ButtonDown(int Player, uint8_t Button)
{
	WriteLED(Player,1 << Button);
	PlayButtonSound(Player,Button);
	P[Player].ButtonIndex = Button;
	P[Player].State = PlayState_waitingForButtonUp;
}

// Restart the round if necessary
void RestartRound()
{
	if (PlayerIsStillPlaying(0))
		P[0].State = PlayState_promptingForNextRound;
	if (PlayerIsStillPlaying(1))
		P[1].State = PlayState_promptingForNextRound;
}

// Handle invalid input
void HandleEvent_InvalidInput(int Player)
{
	PlayMp3(BuzzerSound);
	P[Player].State = PlayState_doneBecauseOfBadEntry;
	RestartRound();
}

// Message timeout to the user
void Message_Timeout()
{
	const char *player = NULL;
	if (P[0].State == PlayState_doneBecauseOfTimeOut)
	{
		if (P[1].State == PlayState_doneBecauseOfTimeOut)
		{
			player = "Both players";
		}
		else
		{
			player = "Left player";
		}
	}
	else
	{
		player = "Right player";
	}
	display.ClearDisplay();
	display.WriteLine("%s TIMEOUT!", player);
}

// Handle a timeout
void HandleEvent_Timeout(int Player)
{
	Log("%s: P0=%s, P0=%s\n", __FUNCTION__, StateString(0), StateString(1));

	// Timeout before we press anything?  This player must not be playing.
	if (CurrentRound == 0 && P[Player].PressCountForThisRound == 0)
	{
		P[Player].State = PlayState_notPlaying;
		// ignore the timeout if the other player is still playing.  
		// If the other player is _not_ playing, then this is a real timeout, even if no keys are pressed.
		if (OtherPlayerIsStillPlaying(Player))
		{
			return;
		}
	}

	// Legitimate timeout.  Let the player know.
	PlayMp3(BuzzerSound);
	P[Player].State = PlayState_doneBecauseOfTimeOut;
	Message_Timeout();
	RestartRound();
}

// Handle valid input
void HandleEvent_ValidInput(int Player)
{
	if (P[Player].PressCountForThisRound == CurrentRound)
	{
		P[Player].State = PlayState_waitingForOtherPlayer;
	}
	else
	{
		P[Player].PressCountForThisRound++;
		P[Player].State = PlayState_waitingForButtonDown;
	}
}

// Handle waiting for button down state
void HandleState_WaitingForButtonDown(int Player)
{
	if (P[Player].State == PlayState_waitingForButtonDown)
	{
		uint8_t button = GetInput(Player);
		if (button == ButtonNotPressed)
		{
			if (millis() - P[Player].TimeStartedWaiting > ButtonPressTimeout)
			{
				HandleEvent_Timeout(Player);
			}
		}
		else if (button == MultipleButtonsPressed)
		{
			HandleEvent_InvalidInput(Player);
		}
		else
		{
			HandleEvent_ButtonDown(Player, button);
		}
	}
}

// Handle a button up event
void HandleEvent_ButtonUp(int Player)
{
	WriteLED(Player, 0);
	if (P[Player].ButtonIndex == Buttons[P[Player].PressCountForThisRound])
	{
		HandleEvent_ValidInput(Player);
	}
	else
	{
		HandleEvent_InvalidInput(Player);
	}
}

// Handle the WaitingForButtonUp state
void HandleState_WaitingForButtonUp(int Player)
{
	if (P[Player].State == PlayState_waitingForButtonUp)
	{
		if (GetInput(Player) == ButtonNotPressed)
		{
			HandleEvent_ButtonUp(Player);
		}
	}
}

// the loop routine runs over and over again forever:
void loop()
{
	HandleState_EndOfRound();

	HandleState_PromptingForNextRound();

	HandleState_WaitingForButtonDown(0);
	HandleState_WaitingForButtonUp(0);

	HandleState_WaitingForButtonDown(1);
	HandleState_WaitingForButtonUp(1);

	if (!PlayerIsStillPlaying(0) && !PlayerIsStillPlaying(1))
	{
		HandleState_DonePlaying();
	}
}

// TODO next: re-prompt on timeout and invlid entry if the other player is still playing


