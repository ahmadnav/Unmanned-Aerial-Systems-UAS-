#include <Arduino.h>
#include<Wire.h>

#define DeviceAddress 0xA0
#define ACC_DATA_Y_MSB 1011
#define ACC_DATA_Y_LSB 1010

void setup() {
	Wire.begin();
	Serial.begin(9600);
}

// the loop function runs over and over again until power down or reset
void loop() {
	Wire.beginTransmission(DeviceAddress);
	Wire.write(ACC_DATA_Y_MSB);
	Wire.write(ACC_DATA_Y_LSB);
	Wire.endTransmission();
	Wire.requestFrom(DeviceAddressG, 2);//Request the 2 incoming bytes.
	int i = 0;
	while (i < 2) {
		Serial.print(Wire.read());
	}
}


