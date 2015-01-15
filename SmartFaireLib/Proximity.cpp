#include "Proximity.h"
#include "arduino.h"

const int ProximityPin = 5;


void Proximity::Init()
{
	pinMode(ProximityPin, INPUT);
}

bool Proximity::ReadSensor()
{
	return digitalRead(ProximityPin) ? true : false;
}