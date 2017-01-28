#pragma once
#include <Arduino.h>
#include <Wire.h> 
#include <Serial.hpp>
#pragma region IMURegisters
#define DeviceAddress 0x1D
#define DeviceRead 0xA7
#define DeviceWrite 0xA6
#define DeviceAddressG 0x53 //General Device Address with no R/W bit specified, R/W specified by arduino function wireread/right.
#define DataX0 0x32
#define DataX1 0x33
#define DataY0 0x33
#define DataY1 0x34
#define DataZ0 0x32
#define DataZ1 0x33
#define Power_Register 0x2D // Power Control Register
#pragma endregion
byte DataX, DataY;




struct inc_messageprop {//Struct to contain properties of incoming message
	int mess_ID;//LLocation of message ID
	int l_CU;//Location of upper Check sum
	int l_CL;//Location of Lower Check sum within a byte array
	int req_ID;//Requested message ID.
};


class CommandProcess {
public:
	byte outgoingmessage[5];
	const byte IMUID = 1010;//0x0A in BYTE (check IMU)
	const byte GPSID = 1110;//0x0A in BYTE (check GPS)
	inc_messageprop incmessageprop = { 0,3,4,2 };
	bool handlemessage(byte* message);
	void getIMUData();
	void getGPSData();
};