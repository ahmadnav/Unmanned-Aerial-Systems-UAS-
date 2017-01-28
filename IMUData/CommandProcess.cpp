#include <Arduino.h>
#include <CommandProcess.hpp>
#include <Serial.hpp>


bool CommandProcess::handlemessage(byte* message) {
	const byte IMUID = 1010;//0x0A in BYTE (check IMU)
	const byte GPSID = 1110;//0x0A in BYTE (check GPS)
	switch (message[incmessageprop.req_ID])
	{
	case IMUID:
		getIMUData();
		return true;
		break;
	case GPSID:
		getGPSData();
		return true;
		break;
	default:
		return false;
		break;
	}
}

void CommandProcess::getIMUData() {
	//Obtains and stores IMU Data within the IMUData array
	int datalength = 15;
	Wire.beginTransmission(DeviceAddressG);
	Wire.write(Power_Register);//Write to power Register
	Wire.write(8); //0000 1000 to enabling measurements
	Wire.endTransmission();

	//The lat data
	Wire.beginTransmission(DeviceAddressG);
	//Device address with read bit on //Add any Required IMU data here
	Wire.write(DataX0);//Request from these registers
	Wire.write(DataZ1);
	Wire.endTransmission();
	Wire.requestFrom(DeviceAddressG, datalength);//Request the 2 incoming bytes.
	int i = 0;
	while (i < datalength) {
		outgoingmessage[i] = Wire.read();
	}

	//The Longitudinal data
	Wire.beginTransmission(DeviceAddressG);//Device address with read bit on add any Required IMU data here
	Wire.write(DataX0);//Request from these registers
	Wire.write(DataZ1);
	Wire.endTransmission();
	Wire.requestFrom(DeviceAddressG, datalength);//Request the 2 incoming bytes.

												 //The Longitudinal data
	i = 0;
	while (i < datalength) {
		outgoingmessage[i]  = Wire.read();//Read the 2 incoming bytes.
	}
}

void CommandProcess::getGPSData() {

}