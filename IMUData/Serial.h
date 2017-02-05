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
	static const int MAX_MESSAGE_SIZE = 0xFF;//Length of byte array of incoming message from GCD
	static const int size_outgoingGPS = 11; //Size of Array carrying outgoing GPSData
	static const int size_outgoingIMU = 16;//Size of Array carrying outgoing IMUData

	typedef struct message_buffer
	{
		uint8_t message[MAX_MESSAGE_SIZE];//Incoming Message
		uint8_t size;
	} message_buffer;


	message_buffer rx_message;//Recieve message
	message_buffer decoded_rx_message;//Decoded recieve message
	message_buffer tx_message;//Transmit Message.
	message_buffer decoded_tx_message;
	/*Outgoing Message Properties*/
	/*uint8_t outgoingmessage[MAX_MESSAGE_SIZE];//Outgoing message.
	uint8_t message[MAX_MESSAGE_SIZE];//Incoming Message
	uint8_t message_size;
	uint8_t dec_mess[size_message];//Decoded in ComingMessage*/

	const byte MessageID = 1000;//0x0A in BYTE (message address to us)
	inc_messageprop incmessageprop = { 0,3,4,2 };

	//Function responsilbe for obtaining data from serial, decoding it and store it in dec_message.
	bool getmessage();
	
	//Function which prints data passed to it.
	void printdata(byte* data);
	

	size_t encode_COBS(const uint8_t* source, size_t size, uint8_t* destination);
	
	//Encodes an message to dec_message in COBS.
	void decode_COBS();
	

	int getEncodedBufferSize(int sourceSize);//Minimum size for encodded buffer 

	uint16_t Fletcher16(uint8_t *data, int count);

	uint16_t mess_checksum16();
	//Function which calculates the 16 bit checksum of message array.

	uint16_t IMUData_checksum16();
	uint16_t GPSData_checksum16();

	bool val_checksum();
	//Validate the checksum data of incoming message, returns true if calculated checksum is equal to the one attatched 
	//to message.

	bool contains(byte* data, byte check, int checklocation);
	//A function which checks for a given byte at a location within a provided byte array(data).

	bool validatemessage();
	//A function whic returns true if the incoming message is directed to us and its fletcher checksum is equal
	//to calculated by us.

	//Sends the recieved message.
	void sendmessage(byte* message);
};