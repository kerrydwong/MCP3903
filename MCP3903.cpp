/*
 * MCP3903 Library
 * Kerry D. Wong
 * http://www.kerrywong.com
 * 5/2014
 */

#include "MCP3903.h"
#include <SPI.h>

MCP3903::MCP3903()
{
    pinMode(pinMOSI, OUTPUT);
    pinMode(pinMISO, INPUT);
    pinMode(pinSPIClock, OUTPUT);
    pinMode(pinCS, OUTPUT);

    digitalWrite(pinCS, HIGH);
}


//set to 24 bit mode
void MCP3903::reset()
{
    reset(OSR_256);
}

//set to resolution specified by OSR
//OSR_32  11 = 256
//OSR_64  10 = 128
//OSR_128 01 = 64 (POR default)
//OSR_256 00 = 32
void MCP3903::reset(byte osr)
{
	unsigned long cmd1 = 0xfc0fd0;
	unsigned long cmd2 = 0x000fc0 | osr << 4;
	byte cmdByte = 	DEVICE_ADDR | REG_CONFIG << 1;

	writeRegister(REG_CONFIG, cmd1);
	writeRegister(REG_CONFIG, cmd2);
}

//read from specified register
//returns 24 bit data
unsigned long MCP3903::readRegister(byte reg)
{
	byte cmdByte = DEVICE_ADDR | reg <<1 | 1;
	unsigned long r = 0;

	digitalWrite(pinCS, LOW);
	SPI.transfer(cmdByte);
	r = (unsigned long) SPI.transfer(0x0) << 16;
	r |= (unsigned long) SPI.transfer(0x0) << 8;
	r |= (unsigned long) SPI.transfer(0x0);
	digitalWrite(pinCS, HIGH);

	return r;
}

//write 24 bit data to register
void MCP3903::writeRegister(byte reg, unsigned long data)
{
	byte cmdByte = DEVICE_ADDR | reg <<1;
	
	byte b2 = (data & 0xff0000) >> 16;
	byte b1 = (data & 0x00ff00) >> 8;
	byte b0 = data & 0x0000ff;

	digitalWrite(pinCS, LOW);
	SPI.transfer(cmdByte);
	SPI.transfer(b2);
	SPI.transfer(b1);
	SPI.transfer(b0);
	digitalWrite(pinCS, HIGH);	
}

//read from ADC channel (0-5)
double MCP3903::readADC(byte channel)
{
	long r = (long) readRegister(channel);

	if (r > 8388607l) r -= 16777216l;
	
	return (double) r / 8388608.0 /3.0;
}

//set channel gain
//channel (0-5)
//valid gain settings:
//GAIN_1
//GAIN_2
//GAIN_4
//GAIN_8
//GAIN_16
//GAIN_32
void MCP3903::setGain(byte channel, byte gain) 
{
	setGain(channel, gain, 0);
}

//overloaded setGain
//the boost parameter indicates the current boost setting for channel
void MCP3903::setGain(byte channel, byte gain, byte boost)
{
	unsigned long r = readRegister(REG_GAIN);

	byte idx = channel * 4;
	unsigned long chGain = 0;

	if (channel % 2 == 0) //0, 2, 4
	{
		chGain = (boost << 3) | gain;
	}
	else //1, 3, 5 
	{
		chGain = boost | (gain << 1);
	}
	
	r &= ~(0xf << idx);
	r |= chGain << idx;

	writeRegister(REG_GAIN, r);
}
