#include <Arduino.h>
#include <Wire.h>
#include<Serial.hpp>


bool serial::getmessage() {
	//Function fills the provided array with data from incoming serial if there is a serial available other wise returns
	//false

	if (Serial.available()) {
		if (Serial.read() - '0' == 0) {// Checks for Start of a instruction if Serial is 0 new instruction available. The 0 value is discarded

			int i = 0;
			while (Serial.available()) {

				if (i < 255) {
					if (Serial.peek() - '0' == 0) {
						//If the current read is a 0 than we are at the end of the message. The peek function doesn't 
						//delete the byte so it gets stored in data.
						Serial.read();//Delete 0 from buffer if we enter this loop.

						serialobj.decode(&message[0], size_message, &dec_mess[0]);
						//Decode the incoming COBs message and store it in decoded message.

						return true;//Get out of While Loop
					}
					message[i] = Serial.read() - '0';//Store the incoming bytes in array
					i++;
				}
				else //If we are greater than 255 bytes read and delete incoming message till its end 
				{
					if (Serial.read() - '0' == 0) {
						//If the current read is a 0 than we are at the end of the message. 
						return false;//Get out of the function, and return false (!no message)
					}
				}

				return true;//Return true, and go ahead on 
				Serial.flush();//Flushes Serial buffer After finishing Storing Instruction
			}
		}
	}
	else
	{
		return false;
	}
}

void serial::printdata(byte* data) {
	//Print the provided byte array in decimals
	int d_size = sizeof(data);
	for (int k = 0; k < d_size; k++) {
		Serial.print(data[k], DEC);
		Serial.print("\n");
	}Serial.print("End of Message"); Serial.print("\n");
}

/// \brief Encode a byte buffer with the COBS encoder.
/// \param source The buffer to encode.
/// \param size The size of the buffer to encode.
/// \param destination The target buffer for the encoded bytes.
/// \returns The number of bytes in the encoded buffer.
/// \warning destination must have a minimum capacity of
///     (size + size / 254 + 1).
size_t serial::encode(const uint8_t* source, size_t size, uint8_t* destination)
{
	size_t read_index = 0;
	size_t write_index = 1;
	size_t code_index = 0;
	uint8_t code = 1;

	while (read_index < size)
	{
		if (source[read_index] == 0)
		{
			destination[code_index] = code;
			code = 1;
			code_index = write_index++;
			read_index++;
		}
		else
		{
			destination[write_index++] = source[read_index++];
			code++;

			if (code == 0xFF)
			{
				destination[code_index] = code;
				code = 1;
				code_index = write_index++;
			}
		}
	}

	destination[code_index] = code;

	return write_index;
}

/// \brief Decode a COBS-encoded buffer.
/// \param source The COBS-encoded buffer to decode.
/// \param size The size of the COBS-encoded buffer.
/// \param destination The target buffer for the decoded bytes.
/// \returns The number of bytes in the decoded buffer.
/// \warning destination must have a minimum capacity of size.
size_t serial::decode(const uint8_t* source, size_t size, uint8_t* destination)
{
	if (size == 0)
		return 0;

	size_t read_index = 0;
	size_t write_index = 0;
	uint8_t code;
	uint8_t i;

	while (read_index < size)
	{
		code = source[read_index];

		if (read_index + code > size && code != 1)
		{
			return 0;
		}

		read_index++;

		for (i = 1; i < code; i++)
		{
			destination[write_index++] = source[read_index++];
		}

		if (code != 0xFF && read_index != size)
		{
			destination[write_index++] = '\0';
		}
	}

	return write_index;
}
/// \Get the maximum encoded buffer size needed for a given source size.
/// \param sourceSize The size of the buffer to be encoded.
/// \returns the maximum size of the required encoded buffer.

int serial::getEncodedBufferSize(int sourceSize)
{
	return sourceSize + sourceSize / 254 + 1;
}

//A function which computes the flecture checksum of a provided byte array, and its size.
uint16_t serial::Fletcher16(uint8_t *data, int count)
{
	uint16_t sum1 = 0;
	uint16_t sum2 = 0;
	int index;
	for (index = 0; index < count; ++index)
	{
		sum1 = (sum1 + data[index]) % 255;
		sum2 = (sum2 + sum1) % 255;
	}
	return (sum2 << 8) | sum1;
}

uint16_t serial::checksum16(byte *arr, int start, int end) {
	int i = start;
	byte checksumarr[30];
	while (i < end) {
		checksumarr[i];
	}
	uint16_t c_sum = Fletcher16(&(checksumarr[0]), size_message);
	return c_sum;
}
bool serial::val_checksum(byte *arr) {
	uint16_t csum;//Total 16bit check sum
	uint8_t c0, c1, f0, f1;
	uint16_t c_sum = checksum16(arr, 0, incmessageprop.l_CU);
	//Calculate the check sum of incoming message of requested ID.
	f0 = csum & 0xff;
	f1 = (csum >> 8) & 0xff;
	c0 = 0xff - ((f0 + f1) % 0xff);//Check Sum Upper 
	c1 = 0xff - ((f0 + c0) % 0xff);//Check SUm Lower
	if (arr[incmessageprop.l_CU] == c0 && arr[incmessageprop.l_CL] == c1) {
		return true;
	}
	else
	{
		return false;
	}

}

bool serial::contains(byte* data, byte check, int checklocation) {
	//A function which checks for a given byte at a location within a provided byte array(data).
	if (data[checklocation] == check) {
		//If byte at location is what it should be return true.
		return true;
	}
	else {
		return false;
	}
}

bool serial::validatemessge() {
	if (val_checksum(&dec_mess[0]) && contains(&(dec_mess[0]), MessageID, incmessageprop.mess_ID)) {
		//if Validitate the checksum equals to what we calculate and message ID is addressed to us return true
		return true;
	}
	else {//Message not intended for us
		return false;//;//Leave Function
	}
}

void serial::sendmessage(byte* message ) {
	const int size = sizeof(message) + 2;
	//size of COBs encoded buffer is size of non encoded buffer +1 
	byte outgoingmessage[size];
	encode(message, sizeof(message), outgoingmessage);
	Serial.write(message, sizeof(message));
}