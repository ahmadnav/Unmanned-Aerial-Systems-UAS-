#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Serial.h>


bool serial::getmessage() {
	//Function fills the provided array with data from incoming serial if there is a serial available other wise returns
	//false

	if (Serial.available() && Serial.read() == 0) {
	// Checks for Start of a instruction if Serial is 0 new instruction available. The 0 value is discarded.

			int i = 0;
			while (Serial.available()) {

				if (i < MAX_MESSAGE_SIZE) {

					if (Serial.peek() == 0) {
						//If the current read is a 0 than we are at the end of the message. The peek function doesn't 
						//delete the byte so it gets stored in data.
						
						rxtx_message.message[i] = Serial.read();//Delete 0 from buffer and store it in message.
						Serial.println("Leaving Loop");
						rxtx_message.size = i+1;
						decode_COBS();
						//Decode the incoming COBs message and store it in decoded message.

						return true;//Get out of While Loop
					}
					Serial.println("Storing Data");
					rxtx_message.message[i] = Serial.read();//Store the incoming bytes in array
					i++;
				}
				else //If we are greater than 255 bytes read and delete incoming message till its end 
				{
					if (Serial.read() == 0) {
						//If the current read is a 0 than we are at the end of the message. 
						return false;//Get out of the function, and return false (!no message)
					}
				}

			} Serial.print("Serial Not Available");
		
		
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
/// Encode the COBS for the outgoing message
void serial::encode_COBS(const uint8_t* source)
{
	int size = source[loc_messagelength];//The length of source file is stored in the second byte of byte array
	size_t read_index = 0;
	size_t write_index = 1;
	size_t code_index = 0;
	uint8_t code = 1;

	while (read_index < size)
	{
		if (source[read_index] == 0)
		{
			rxtx_message.message[code_index] = code;
			code = 1;
			code_index = write_index++;
			read_index++;
		}
		else
		{
			rxtx_message.message[write_index++] = source[read_index++];
			code++;

			if (code == 0xFF)
			{
				rxtx_message.message[code_index] = code;
				code = 1;
				code_index = write_index++;
			}
		}
	}

	rxtx_message.message[code_index] = code;

}

/// \brief Decode a COBS-encoded buffer.
/// \param source The COBS-encoded buffer to decode.
/// \param size The size of the COBS-encoded buffer.
/// \param destination The target buffer for the decoded bytes.
/// \returns The number of bytes in the decoded buffer.
/// \warning destination must have a minimum capacity of size.
void serial::decode_COBS()
{
	size_t read_index = 0;
	size_t write_index = 0;
	uint8_t code;
	uint8_t i;

	while (read_index < rxtx_message.size)
	{
		code = rxtx_message.message[read_index];

		if (read_index + code > rxtx_message.size && code != 1)
		{
			break;
		}

		read_index++;

		for (i = 1; i < code; i++)
		{
			decoded_rx_message.message[write_index++] = rxtx_message.message[read_index++];
		}

		if (code != 0xFF && read_index != rxtx_message.size)
		{
			//decoded_rx_message.message[write_index++] = '\0';
		}

	}

	decoded_rx_message.size = write_index;
	return;
}
/// \Get the maximum encoded buffer size needed for a given source size.
/// \param sourceSize The size of the buffer to be encoded.
/// \returns the maximum size of the required encoded buffer.

int serial::getEncodedBufferSize(int sourceSize)
{
	return sourceSize + sourceSize / 254 + 1;
}

//A function which computes the flecture checksum of a provided byte array, and uptill size.
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

/*Validate the checksum of incoming message*/
bool serial::val_checksum() {
	uint16_t received_message_checksum = Fletcher16(decoded_rx_message.message, (decoded_rx_message.size - 2));
	uint16_t checksum = (decoded_rx_message.message[decoded_rx_message.size - 2] << 8) | decoded_rx_message.message[decoded_rx_message.size - 1];

	if (received_message_checksum == checksum) {
		Serial.println("CheckSums Are Equal");
		return true;
	}
	else {
		return false;
	}
}

/*Ensure that the message is addressed to us*/
bool serial::contains(byte* data, byte check, int checklocation) {
	//A function which checks for a given byte at a location within a provided byte array(data).
	if (data[checklocation] == check) {
		//If byte at location is what it should be return true.
		
		return true;
	}
	else {
		Serial.println(data[checklocation]);
		Serial.println(check);
		Serial.println("Message Not Address To Sensors");
		return false;
	}
}

/*Ensures that the message recieved is addressed to sensors, and that its checksum is correct*/
bool serial::validatemessage() {
	if (val_checksum() & contains(decoded_rx_message.message, MessageID, incmessageprop.mess_ID)) {
		//if Validitate the checksum equals to what we calculate and message ID is addressed to us return true
		return true;
	}
	else {//Message not intended for us
		Serial.println("ValidateMessageFailed.");
		return false;//;//Leave Function
	}
}

/*Calculated the checksum, appends it to out going message and Sends an out going message recieved from commandprocess*/
void serial::sendmessage(byte* message) {
	//message[]
	int size = message[loc_messagelength];//Size of the array recieved

	uint16_t checksum = Fletcher16(message, size);
	//Append the lower and upper CheckSums
	for (int c = 1; c < 3; c++) {
		size++;//Increase the size of array by one (for the Cecksums)
		message[size - 1] = (checksum >> (c*8)) && 0xFF;//Appends Lower And Upper checksums
	}
	
	encode_COBS(message);

	Serial.write(rxtx_message.message, (message[loc_messagelength]));//Send the message of length over serial.
}