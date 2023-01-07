#include <cstring>
#include <stdio.h>

#define MSB 0xEB
#define LSB 0x90
#define CHK 32
#define DATA_LEN 32

u_int8_t dummy_data[] = {0xEB, 0x90, 3,	 4,	 5,	 6,	 7,	 8,	 9,	 10, 11, 12, 13, 14, 15, 16,
						 17,   18,	 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};

int counter = 0;
bool have_msb = false;
bool have_lsb = false;
u_int8_t data[32];

enum State { SYNC1, SYNC2, PAYLOAD, CHECKSUM };

State state = SYNC1;

void resetParser() {
	counter = 0;
	memset(&data, 0, sizeof(data));
	state = SYNC1;
}

int addToArray(u_int8_t b) {
	data[counter] = b;
	counter++;
	if (counter == DATA_LEN - 1) {
		return 1;
	}
	return 0;
}

void rxDone() {
	for (int i = 0; i < DATA_LEN; i++) {
		printf("%d ", data[i]);
	}
	printf("\n");
}

void parseData(u_int8_t b) {

	switch (state) {
	case SYNC1:
		if (b == MSB) {
			state = SYNC2;
			addToArray(b);
		}
		break;

	case SYNC2:
		if (b == LSB) {
			state = PAYLOAD;
			addToArray(b);
		} else {
			resetParser();
		}
		break;

	case PAYLOAD:
		if (!addToArray(b)) {
			// do nothing payload waiting
		} else {
			state = CHECKSUM;
		}
		break;

	case CHECKSUM:
		if (b == CHK) {
			addToArray(b);
			rxDone();
		}
		resetParser();
		break;

	default:
		break;
	}
}

int main() {
	for (int j = 0; j < DATA_LEN; j++)
		for (int i = 0; i < DATA_LEN; i++) {
			parseData(dummy_data[i]);
		}

	return 0;
}
