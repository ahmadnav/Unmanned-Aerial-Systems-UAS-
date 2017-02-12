

// the setup function runs once when you press reset or power the board
#include <IMUData.h>
#include <SoftwareSerial.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_GPS.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "Serial.h"
#include <CommandProcess.h>

IMUData imudata;
serial serialobj;
CommandProcess CommandProcessObj;

byte check_mess[5] = { 8,5,4,0,0 };
byte checksum;
void setup() {
	Wire.begin();
	Serial.begin(57600);
}

// the loop function runs over and over again until power down or reset
void loop() {
	CheckSum();
	CheckSerial();
}
void CheckSum() {
	uint16_t c_sum =  serialobj.Fletcher16(check_mess, 3);
	Serial.print(c_sum,DEC);
}
void CheckSerial() {
	if (serialobj.getmessage()) {
		//Obtains the message from serial stores it in IMUdata and stores the message recieved and decodes it, storing it
		//in dec_message
		
		if (serialobj.validatemessage) {
			//Only Continue getting data if the flexture checksum computed is equal to the one provided within the 
			//incoming message is addressed to us.

			if (CommandProcessObj.handlemessage(serialobj.decoded_rx_message.message)) {

				serialobj.sendmessage(&CommandProcessObj.outgoingmessage[0]);
			}
			else {
				return;
			}
		}
		else {
			return;
		}

	}
	else {
		return;
	}
}
void RecieveAndSendMessage() {
	if (serialobj.getmessage()) {
		//Obtains the message from serial stores it in IMUdata and stores the message recieved and decodes it, storing it
		//in dec_message

		if (serialobj.validatemessage) {
			//Only Continue getting data if the flexture checksum computed is equal to the one provided within the 
			//incoming message is addressed to us.

			if (CommandProcessObj.handlemessage(serialobj.decoded_rx_message.message)) {

				
				serialobj.sendmessage(CommandProcessObj.outgoingmessage);
			}
			else {
				return;
			}
		}
		else {
			return;
		}

	}
	else {
		return;
	}

}