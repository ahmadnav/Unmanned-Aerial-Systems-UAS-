#pragma once
#ifndef IMUData_h
#define IMUData_h
#include "Arduino.h"
#include <Wire.h>

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

#define IMUCheck 0x0A // The instruction to check IMU data(0x0A)


static const int size_message = 5;//Length of byte array of incoming message from GCD
static const int size_data = 34;//Length of Byte array to send out

struct s_message {//Struct to contain properties of incoming message
	int m_ID;//LLocation of message ID
	int l_CU;//Location of upper Check sum
	int l_CL;//Location of Lower Check sum within a byte array
	int r_ID;//Requested message ID.
};
struct s_IMUData
{//Struct to contain properties of outgoing message
	int m_ID;//Location of message ID
	int l_CU;//Location of Upper Checksum
	int l_CL;
	int p_lat;//Location of pay load Latitude
	int p_long; //Location of payload Longitude
	int l_paylload;//Location of Payload length
	int length_payload;
};
class IMUData {//Serial controller, 
public:
	const byte CheckIMU = 1010;//0x0A in BYTE (check IMU)
	int pay_length = 30;//Initialize Payload Length
	s_IMUData IMUData_s = {0,33,32,2,17,1,15};//Initialize IMUData struct
	s_message message_s = {0,3,4,2};

	byte message[size_message];//Incoming Message
	uint8_t dec_mess[size_message];//Decoded message

	byte IMUData[size_data];//The byte array which is to be sent out with the IMUData.
	byte enc_IMUData[size_data + 1];// Encode IMU data

	bool c_message;//True when we are processing a message
	int d_size;//Length of bytes recieved
	bool getmessage(byte *data);
	void printdata(byte* data);
	bool contains(byte* data, byte check, int checklocation);
	//A function which checks for a given byte at a location within a provided byte array(data).
	size_t encode(const uint8_t* source, size_t size, uint8_t* destination);
	//Encodes an array of byte to from source to destination of size.
	size_t decode(const uint8_t* source, size_t size, uint8_t* destination);
	int getEncodedBufferSize(int sourceSize);//Minimum size for encodded buffer
	uint16_t Fletcher16(uint8_t *data, int count);
	void append(uint8_t* main, uint16_t* append);//Function appends the append files at a provided location.
	void getIMUData();
	uint16_t checksum16(byte *arr, int start, int end);
	//Function which calculates the 16 bit checksum of an array from start till end.
	bool val_checksum(byte *arr);
	//Validate the checksum data of incoming message, returns true if calculated checksum is equal to the one attatched to message.

};

#endif // !IMUData_h
