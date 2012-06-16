/*
 *  OpenSunny -- OpenSource communication with SMA Readers
 *
 *  Copyright (C) 2012 Christian Simon <simon@swine.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program (see the file COPYING included with this
 *  distribution); if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <string.h>
#include <stdio.h>

#include "opensunny.h"

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
