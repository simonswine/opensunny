/*
 * in_bluetooth.c
 *
 *  Created on: 08.06.2012
 *      Author: christian
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include "logging.h"
#include "utils.h"
#include "in_bluetooth.h"

void in_bluetooth_connect(struct bluetooth_inverter * inv) {
	struct sockaddr_rc addr = { 0 };

	inv->l2_packet_send_count = 1;

	inv->socket_fd = 0;
	// allocate a socket
	inv->socket_fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	// set the connection parameters (who to connect to)
	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = (uint8_t) 1;
	str2ba(inv->macaddr, &addr.rc_bdaddr);

	// connect to server
	inv->socket_status = connect(inv->socket_fd, (struct sockaddr *) &addr,
			sizeof(addr));

	if (inv->socket_status < 0) {
		log_debug(
				"[BT] Connection to inverter %s failed: %s", inv->macaddr, strerror(errno));
	}

}

int in_bluetooth_write(struct bluetooth_inverter * inv, unsigned char * buffer,
		int len) {
	char buffer_hex[len * 3];
	int status = write(inv->socket_fd, buffer, len);

	buffer_hex_dump(buffer_hex, buffer, len);
	log_debug("[BT] Sent %d bytes: %s", len, buffer_hex);

	return status;

}

int in_bluetooth_connect_read(struct bluetooth_inverter * inv) {

	char buffer_hex[BUFSIZ * 3];
	int count;

	count = read(inv->socket_fd, inv->buffer, BUFSIZ);

	if (count > 0) {
		buffer_hex_dump(buffer_hex, inv->buffer, count);
		log_debug("[BT] Received %d bytes: %s", count, buffer_hex);
	}

	inv->buffer_len = count;
	inv->buffer_position = 0;

	return count;

}

/* Get my mac address */
void in_bluetooth_get_my_address(struct bluetooth_inverter * inv,
		unsigned char * addr) {

	/* Get my Mac */
	unsigned int eight = 8;
	unsigned char mymac[8] = { 0 };
	getsockname(inv->socket_fd, &mymac, &eight);

	/* Copy to Buffer */
	memcpy(addr, mymac + 2, 6);

	/* Reverse Buffer */
	buffer_reverse(addr, 6);

}

/* fetch one byte from stream */
char in_bluetooth_get_byte(struct bluetooth_inverter * inv) {

	/* Check if its neccessary to fetch new buffer content */
	while (inv->buffer_len == 0 || inv->buffer_len <= inv->buffer_position) {
		in_bluetooth_connect_read(inv);
	}

	return inv->buffer[inv->buffer_position++];

}

/* fetch multiple bytes */
void in_bluetooth_get_bytes(struct bluetooth_inverter * inv,
		unsigned char *buffer, int count) {
	for (int i = 0; i < count; ++i) {
		if (buffer == NULL)
			in_bluetooth_get_byte(inv);
		else
			buffer[i] = in_bluetooth_get_byte(inv);
	}
}

