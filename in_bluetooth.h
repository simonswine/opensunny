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

#ifndef OPENSUNNY_IN_BLUETOOTH_H_
#define OPENSUNNY_IN_BLUETOOTH_H_

#include <stdio.h>
#include "in_smadata2plus_structs.h"

struct bluetooth_inverter {
	char name[32];
	char macaddr[18];
	unsigned char password[13];
	int socket_fd;
	int socket_status;
	unsigned char buffer[BUFSIZ];
	int buffer_len;
	int buffer_position;
	int l2_packet_send_count;
	unsigned int serial;
	struct smadata2_model *model;
};

void in_bluetooth_connect(struct bluetooth_inverter * inv);
int in_bluetooth_connect_read(struct bluetooth_inverter * inv);
char in_bluetooth_get_byte(struct bluetooth_inverter * inv);
void in_bluetooth_get_bytes(struct bluetooth_inverter * inv,
		unsigned char *buffer, int count);
int in_bluetooth_write(struct bluetooth_inverter * inv, unsigned char * buffer,
		int len);
void in_bluetooth_get_my_address(struct bluetooth_inverter * inv,
		unsigned char * addr);

#endif /* OPENSUNNY_IN_BLUETOOTH_H_ */
