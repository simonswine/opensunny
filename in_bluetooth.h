/*
 * in_bluetooth.h
 *
 *  Created on: 08.06.2012
 *      Author: christian
 */

#ifndef IN_BLUETOOTH_H_
#define IN_BLUETOOTH_H_

struct bluetooth_inverter {
	char name[32];
	char macaddr[18];
	int socket_fd;
	int socket_status;
	unsigned char buffer[BUFSIZ];
	int buffer_len;
	int buffer_position;
	int l2_packet_send_count;
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
#endif /* IN_BLUETOOTH_H_ */
