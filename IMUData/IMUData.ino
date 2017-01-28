

// the setup function runs once when you press reset or power the board
#include <Wire.h>
#include <Serial.hpp>
#include<CommandProcess.hpp>
serial serialobj;
CommandProcess CommandProcessObj;
void setup() {
	Wire.begin();
	Serial.begin(57600);
}

// the loop function runs over and over again until power down or reset
void loop() {

}
void RecieveAndSendMessage() {
	if (serialobj.getmessage()) {
		//Obtains the message from serial stores it in IMUdata and stores the message recieved and decodes it, storing it
		//in dec_message

		if (serialobj.validatemessge) {
			//Only Continue getting data if the flexture checksum computed is equal to the one provided within the 
			//incoming message is addressed to us.

			CommandProcessObj.handlemessage(serialobj.dec_mess);
			//The handle message function gets the approprate data.
			if (CommandProcessObj.handlemessage(serialobj.dec_mess)) {
				serialobj.sendmessage(&CommandProcessObj.outgoingmessage[0]);
				//IMUdata.getIMUData();//Obtains IMUData and stores it in IMUdata.IMUData byte array at the correct location.
				//IMUdata.IMUData[IMUdata.IMUData_s.m_ID] = IMUdata.CheckIMU;//The Message ID
				//IMUdata.IMUData[IMUdata.IMUData_s.l_paylload] = IMUdata.pay_length;//PayLoad Length
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