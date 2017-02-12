#pragma once
#include <Arduino.h>
#include <CommandProcess.h>

#pragma region IMUSetup
Adafruit_BNO055 bno = Adafruit_BNO055(55); //SCL to pin A5, SDA to pin A4
#pragma endregion

#pragma region GPSSetup
/* Serial corresponds to 0 RX and 1 TX on Arduino UNO */
#define GPSSerial Serial
/* Connect to the GPS on the hardware port */
Adafruit_GPS GPS(&GPSSerial);
#pragma endregion

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
		IMU_x[i] = (IMUx >> (maxbyteIMU - i) * 8) && 0xFF;
		IMU_y[i] = (IMUy >> (maxbyteIMU - i) * 8) && 0xFF;
		IMU_z[i] = (IMUz >> (maxbyteIMU - i) * 8) && 0xFF;
	}
}

void CommandProcess::getGPSData() {
	GPS.begin(9600);
	int GPSlat = GPS.latitude * (10^GPS_precision);
	int GPSlong = GPS.longitude * (10^GPS_precision);

	/*Convert int to byte array MSB to LSB*/
	for (int i = 0; i <= maxbyteGPS; i++) {
		GPS_lat[i] = (GPSlat >> (maxbyteGPS - i) * 8) && 0xFF;
		GPS_long[i] = (GPSlong >> (maxbyteGPS - i) * 8) && 0xFF;
	}

}

void CommandProcess::packoutgoingmessage(byte MessageID) {
	int k = 0; //Array field incrementor
	switch (MessageID)
	{
	case IMUID:
		
		outgoingmessage[k] = IMUID;//Message ID
		k = k + 2;//Increment IMUID, and the length byte(updated at end)
		/// Insert the IMUData into outgoing array
		insertbytearray(outgoingmessage, IMU_x, k, maxbyteIMU);//Insert the x orientation of IMU
		k += maxbyteIMU;
		insertbytearray(outgoingmessage, IMU_y, k, maxbyteIMU);//Insert the y orientaion of IMU
		k += maxbyteIMU;
		insertbytearray(outgoingmessage, IMU_z, k,maxbyteIMU);//Insert the y orientaion of IMU
		k += maxbyteIMU;
		///
		outgoingmessage[1] = k++; //Length of outgoing IMUmessage without the 2 checksum bytes
		break;
	case GPSID:
		
		outgoingmessage[k] = GPSID;//Message ID
		k += 2;//Increment IMUID, and the length byte(updated at end)
		
		insertbytearray(outgoingmessage, GPS_long, k, maxbyteGPS);//Insert the longitude
		k += maxbyteGPS;
		insertbytearray(outgoingmessage, GPS_lat, k, maxbyteGPS);//Insert the latitude
		k += maxbyteGPS;

		outgoingmessage[1] = k++; // Update the outgoing message length.
		break;
	default:
		break;
	}
}

/*Insert ins_arr of size size in arr at ins_loc*/
void CommandProcess::insertbytearray(byte* arr, byte* ins_arr,int ins_loc,int size) {
	
	for (int i = 0; i < size; i++) {
		arr[ins_loc] = ins_arr[i];
		ins_loc = ins_loc + 1;//Increment insert location.
	}
	return;
}