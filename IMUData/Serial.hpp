#pragma once
#include <Arduino.h>
#include <Wire.h>

//static const int size_message = 5;//Length of byte array of incoming message from GCD
//static const int size_data = 34;//Length of Byte array to send out

class serial {
	struct inc_messageprop {//Struct to contain properties of incoming message
		int mess_ID;//LLocation of message ID
		int l_CU;//Location of upper Check sum
		int l_CL;//Location of Lower Check sum within a byte array
		int req_ID;//Requested message ID.
	};
public: 
	static const int size_message = 5;//Length of byte array of incoming message from GCD
	static const int size_data = 34;//Length of Byte array to send out
	//static const int size_data = 34;//Length of Byte array to send out

	byte message[size_message];//Incoming Message
	uint8_t dec_mess[size_message];//Decoded in ComingMessage

	const byte MessageID = 1010;//0x0A in BYTE (message address to us)
	inc_messageprop incmessageprop = { 0,3,4,2 };

	bool getmessage();
	//Function responsilbe for obtaing data from serial and storing it in an array passed to it.
	void printdata(byte* data);
	//Function which prints data passed to it.
	size_t encode(const uint8_t* source, size_t size, uint8_t* destination);
	//Encodes an array of byte to from source to destination of size.
	size_t decode(const uint8_t* source, size_t size, uint8_t* destination);

	int getEncodedBufferSize(int sourceSize);//Minimum size for encodded buffer 

	uint16_t Fletcher16(uint8_t *data, int count);

	uint16_t checksum16(byte *arr, int start, int end);
	//Function which calculates the 16 bit checksum of an array from array postion start till not including end.

	bool val_checksum(byte *arr);
	//Validate the checksum data of incoming message, returns true if calculated checksum is equal to the one attatched 
	//to message.

	bool contains(byte* data, byte check, int checklocation);
	//A function which checks for a given byte at a location within a provided byte array(data).

	bool validatemessge();
	//A function whic returns true if the incoming message is directed to us and its fletcher checksum is equal
	//to calculated by us.

	void sendmessage(byte* message);
};