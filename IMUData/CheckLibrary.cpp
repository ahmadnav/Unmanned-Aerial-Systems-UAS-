
#include "IMUData.h"

using namespace std;

IMUData imudata;
const int length = 5;
byte data[length];

int main() {
	int i = 0;
	while ( i < 5) {
		printf("Data to append:");
		scanf("%i", &data[i]);
	}
	//imudata.encode();
		
};