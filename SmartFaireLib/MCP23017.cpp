#include "MCP23017.h"
#include "arduino.h"

// TABLE 1-6: CONTROL REGISTER SUMMARY (IOCON.BANK = 0)
const unsigned char IODIRA = 0x00;
const unsigned char IODIRB = 0x01;
const unsigned char IPOLA = 0x02;
const unsigned char IPOLB = 0x03;
const unsigned char GPINTENA = 0x04;
const unsigned char GPINTENB = 0x05;
const unsigned char DEFVALA = 0x06;
const unsigned char DEFVALB = 0x07;
const unsigned char INTCONA = 0x08;
const unsigned char INTCONB = 0x09;
const unsigned char IOCON = 0x0A;
const unsigned char GPPUA = 0x0C;
const unsigned char GPPUB = 0x0D;
const unsigned char INTFA = 0x0E;
const unsigned char INTFB = 0x0F;
const unsigned char INTCAPA = 0x10;
const unsigned char INTCAPB = 0x11;
const unsigned char GPIOA = 0x12;
const unsigned char GPIOB = 0x13;
const unsigned char OLATA = 0x14;
const unsigned char OLATB = 0x15;

MCP23017::MCP23017(unsigned char address)
{
	_address = address;
}

MCP23017::~MCP23017()
{
}

void MCP23017::Init()
{
	_i2c.begin();
}

void MCP23017::SetPortAInputPins(unsigned char pin)
{
	_i2c.beginTransmission(_address);
	_i2c.write(IODIRA);
	_i2c.write(pin);
	_i2c.endTransmission();
	
}

void MCP23017::SetPortBInputPins(unsigned char pin)
{
	_i2c.beginTransmission(_address);
	_i2c.write(IODIRB);
	_i2c.write(pin);
	_i2c.endTransmission();
}

void MCP23017::SetPortAInvert(unsigned char pin)
{
	_i2c.beginTransmission(_address);
	_i2c.write(IPOLA);
	_i2c.write(pin);
	_i2c.endTransmission();
}

void MCP23017::SetPortBInvert(unsigned char pin)
{
	_i2c.beginTransmission(_address);
	_i2c.write(IPOLB);
	_i2c.write(pin);
	_i2c.endTransmission();
}

void MCP23017::SetPortAPullUp(unsigned char pin)
{
	_i2c.beginTransmission(_address);
	_i2c.write(GPPUA);
	_i2c.write(pin);
	_i2c.endTransmission();
}

void MCP23017::SetPortBPullUp(unsigned char pin)
{
	_i2c.beginTransmission(_address);
	_i2c.write(GPPUB);
	_i2c.write(pin);
	_i2c.endTransmission();
}

unsigned char MCP23017::ReadPortA()
{
	_i2c.beginTransmission(_address);
	_i2c.write(GPIOA);
	_i2c.endTransmission();

	_i2c.requestFrom(_address, 1);
	unsigned char value = (unsigned char)_i2c.read();
	return value;
}

unsigned char MCP23017::ReadPortB()
{
	_i2c.beginTransmission(_address);
	_i2c.write(GPIOB);
	_i2c.endTransmission();
	_i2c.requestFrom(_address, 1);
	unsigned char value = (unsigned char)_i2c.read();
	return value;
}

void MCP23017::WritePortA(unsigned char pinToSet)
{
	_i2c.beginTransmission(_address);
	_i2c.write(GPIOA);
	_i2c.write(pinToSet);
	_i2c.endTransmission();
}

void MCP23017::WritePortB(unsigned char pinToSet)
{
	_i2c.beginTransmission(_address);
	_i2c.write(GPIOB);
	_i2c.write(pinToSet);
	_i2c.endTransmission();
}

