#pragma once
#include <Arduino.h>
#include <Wire.h> 
#include "Serial.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_GPS.h>

#pragma region IMUSetup
Adafruit_BNO055 bno = Adafruit_BNO055(55); //SCL to pin A5, SDA to pin A4
const int maxbyteIMU = 4;

#pragma endregion

#pragma region GPSSetup
// Serial corresponds to 0 RX and 1 TX on Arduino UNO
#define GPSSerial Serial
// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);
const int maxbyteGPS = 4;
#pragma endregion


struct s_outgoingmessage
{//Struct to contain byte locations of outgoing message.
	int m_ID;//Location of message ID
	int length;//Location of length byte
	int payload;
	int CU;//Location of Upper Checksum
	int CL;
};


struct inc_messageprop {//Struct to contain properties of incoming message
	int mess_ID;//LLocation of message ID	
	int l_CU;//Location of upper Check sum
	int l_CL;//Location of Lower Check sum within a byte array
	int req_ID;//Requested message ID.
};


class CommandProcess {
public:
	static const uint8_t MAX_SIZE = 0xFF;

	uint8_t outgoingmessage[MAX_SIZE];

	static const byte IMUID = 1010;//0x0A in BYTE (check IMU)
	static const byte GPSID = 1110;//0x0A in BYTE (check GPS)
	inc_messageprop incmessageprop = { 0,3,4,2 };//Properties of incoming message like location of messageID etc.
	
	void getIMUData();
	void getGPSData();
	byte currentMessage;//Logs the current message being processed updated within the processdata function.

	/* x,y,z orientaion of IMU*/
	byte IMU_x[maxbyteIMU];
	byte IMU_y[maxbyteIMU];
	byte IMU_z[maxbyteIMU];
	int IMU_precision = 4;//Decimal prection in floating point
	static const int size_outgoingIMU = 16;//Size of Array carrying outgoing IMUData
	byte outgoingIMU[size_outgoingIMU];//OutgoingIMUMessage
	s_outgoingmessage Index_IMUData = { 0,1,2,13,14 };

	/* Latitude and Longitude of GPS*/
	byte GPS_lat[4];
	byte GPS_long[4];
	int GPS_precision = 4;
	static const int size_outgoingGPS = 11; //Size of Array carrying outgoing GPSData
	byte outgoingGPS[size_outgoingGPS];
	s_outgoingmessage Index_GPSData = { 0,1,2,9,10 };

	/*A function which handles incoming message and obtains/stores values from sensors*/
	bool handlemessage(byte* message);

	/*A function which packs outgoing data for various sensors, takes in MessageID*/
	void packoutgoingmessage(byte MessageID);

	/*Inserts a byte ins_array of size into the array provided starting at insert location*/
	void insertbytearray(byte* arr, byte* ins_arr,int ins_loc);
};

