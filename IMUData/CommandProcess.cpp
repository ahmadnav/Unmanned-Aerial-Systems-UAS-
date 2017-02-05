#include <Arduino.h>
#include <CommandProcess.h>
#include <Serial.h>


bool CommandProcess::handlemessage(byte* message) {
	
	switch (message[incmessageprop.req_ID])
	{
	case IMUID:
		getIMUData();
		currentMessage = IMUID;
		packoutgoingmessage(currentMessage); //Pack the outgoing message.
		return true;
		break;
	case GPSID:
		getGPSData();
		currentMessage = GPSID;
		packoutgoingmessage(currentMessage);//Pack the outgoing message.
		return true;
		break;
	default:
		return false;
		break;
	}
}

void CommandProcess::getIMUData() {
	/* Get a new sensor event */
	sensors_event_t event;
	bno.getEvent(&event);

	/* Get the x,y, and z orientation of the IMU */
	int IMUx = event.orientation.x * (10^IMU_precision);
	int IMUy = event.orientation.y * (10^IMU_precision);
	int IMUz = event.orientation.z * (10^IMU_precision);

	/*Convert int to byte array MSB to LSB*/
	for (int i = 0; i <= maxbyteIMU; i++) {
		IMU_x[i] = (IMUx >> (maxbyteIMU - i) * 8);
		IMU_y[i] = (IMUy >> (maxbyteIMU - i) * 8);
		IMU_z[i] = (IMUz >> (maxbyteIMU - i) * 8);
	}
}

void CommandProcess::getGPSData() {
	int GPSlat = GPS.latitude * (10^GPS_precision);
	int GPSlong = GPS.longitude * (10^GPS_precision);

	/*Convert int to byte array MSB to LSB*/
	for (int i = 0; i <= maxbyteGPS; i++) {
		GPS_lat[i] = (GPSlat >> (maxbyteGPS - i) * 8);
		GPS_long[i] = (GPSlong >> (maxbyteGPS - i) * 8);
	}

}

void CommandProcess::packoutgoingmessage(byte MessageID) {

	switch (MessageID)
	{
	case IMUID:
		outgoingmessage[Index_IMUData.m_ID] = IMUID;//Message ID
		outgoingmessage[Index_IMUData.length] = size_outgoingIMU - 2; //Size of outgoing IMUmessage subtract the 2 checksum bytes
		insertbytearray(&outgoingmessage[0], &IMU_x[0], Index_IMUData.payload);//Insert the x orientation of IMU
		insertbytearray(&outgoingmessage[0], &IMU_y[0], (Index_IMUData.payload + sizeof(IMU_x)));//Insert the y orientaion of IMU
		insertbytearray(&outgoingmessage[0], &IMU_z[0], (Index_IMUData.payload + sizeof(IMU_y)));//Insert the y orientaion of IMU
		break;
	case GPSID:
		outgoingmessage[Index_GPSData.m_ID] = GPSID;//Message ID
		outgoingmessage[Index_GPSData.length] = size_outgoingGPS;
		insertbytearray(&outgoingmessage[0], &GPS_long[0], Index_GPSData.payload);//Insert the longitude
		insertbytearray(&outgoingmessage[0], &GPS_lat[0], (Index_GPSData.payload + sizeof(GPS_long)));//Insert the latitude
		break;
	default:
		break;
	}
}

void CommandProcess::insertbytearray(byte* arr, byte* ins_arr,int ins_loc) {
	int size = sizeof(ins_arr);//length of Insert Array.
	for (int i = 0; i < size; i++) {
		arr[ins_loc] = ins_arr[i];
		ins_loc = ins_loc + 1;//Increment insert location.
	}
	return;
}