#include "utils.h"

void buffer_hex_dump(char * output, unsigned char * buffer, int len) {

	char hex[14];

	output[0] = '\0';

	for (int i = 0; i < len; ++i) {

		sprintf(hex, "%02x:", buffer[i]);
		strncat(output, hex, sizeof(output));
	}

	//remove last colon
	output[strlen(output) - 1] = '\0';

}

void buffer_reverse(unsigned char * buffer, int len) {
	unsigned char temp;

	for (int i = 0; i < len / 2; ++i) {
		int tail = len - i - 1;
		temp = buffer[i];
		buffer[i] = buffer[tail];
		buffer[tail] = temp;
	}

}

void buffer_repeat(unsigned char * buffer, unsigned char c, int count) {
	for (int i = 0; i < count; ++i) {
		buffer[i] = c;
	}
}

// Checks for SMA2+ package 7E FF 03 60 65
int isSMA2plusPackage(unsigned char *package, int psize, int debug) {
	int i;
	if (debug == 1) {
		printf("package: ");
		for (i = 21; i <= 24; i++) {
			printf("%02x ", package[i]);
		}
		printf("\n");
	}
	//debug_printf("package: %02x %02x %02x %02x %02x %02x %02x %02x\n", package[16], package[17], package[18], package[19], package[20], package[21], package[22], package[23]);
	if (package[21] == 0x7E && package[22] == 0xFF && package[23] == 0x03
			&& package[24] == 0x60 && package[25] == 0x65) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/* convert 2 chars to hex */
unsigned char conv(char *nn) {
	unsigned char tt = 0, res = 0;
	int i;

	for (i = 0; i < 2; i++) {
		switch (nn[i]) {

		case 65: /*A*/
		case 97: /*a*/
			tt = 10;
			break;

		case 66: /*B*/
		case 98: /*b*/
			tt = 11;
			break;

		case 67: /*C*/
		case 99: /*c*/
			tt = 12;
			break;

		case 68: /*D*/
		case 100: /*d*/
			tt = 13;
			break;

		case 69: /*E*/
		case 101: /*e*/
			tt = 14;
			break;

		case 70: /*F*/
		case 102: /*f*/
			tt = 15;
			break;

		default:
			tt = nn[i] - 48;
			break;
		}
		res = res + (tt * pow(16, 1 - i));
	}
	return res;
}
