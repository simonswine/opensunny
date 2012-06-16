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

#ifndef OPENSUNNY_IN_SMADATA2PLUS_H_
#define OPENSUNNY_IN_SMADATA2PLUS_H_

#include <stdio.h>
#include "in_bluetooth.h"

#define SMADATA2PLUS_STARTBYTE 0x7e
#define SMADATA2PLUS_L1_HEADER_LEN 18

#define SMADATA2PLUS_L1_CMDCODE_LEVEL2 1		// 0x0001
#define SMADATA2PLUS_L1_CMDCODE_BROADCAST 2  	// 0x0002
#define SMADATA2PLUS_L1_CMDCODE_5 5  			// 0x0005
#define SMADATA2PLUS_L1_CMDCODE_FRAGMENT 8		// 0x0008
#define SMADATA2PLUS_L1_CMDCODE_10 10  			// 0x000a
#define SMADATA2PLUS_L1_CMDCODE_12 12  			// 0x000c

#define SMADATA2PLUS_MAX_VALUES 64

/* level1 packet */
struct smadata2_l1_packet {
	int length;
	unsigned char checksum;
	unsigned char src[6];
	unsigned char dest[6];
	int cmd_code;
	unsigned char content[BUFSIZ];
};

/* level2 packet */
struct smadata2_l2_packet {
	unsigned char ctrl1;
	unsigned char ctrl2;
	unsigned char archcd;
	unsigned char zero;
	unsigned char c;
	unsigned char content[BUFSIZ];
	int content_length;
};

/* smadata2 value */
struct smadata2_value {
	char name[64];
	char unit[5];
	float factor;
	unsigned long value;
	int timestamp;
	int r_value_pos;
	int r_value_len;
	int r_timestamp_pos;

};

/* smadata2 query */
struct smadata2_query {
	unsigned char q_ctrl1;
	unsigned char q_ctrl2;
	unsigned char q_archcd;
	unsigned char q_zero;
	unsigned char q_c;
	unsigned char q_content[128];
	int q_content_length;
	unsigned char r_ctrl1;
	unsigned char r_ctrl2;
	struct smadata2_value values[12];
	int value_count;
};



void in_smadata2plus_level1_cmdcode_wait(struct bluetooth_inverter * inv,
		struct smadata2_l1_packet *p, struct smadata2_l2_packet * p2 , int cmdcode);

int in_smadata2plus_level1_packet_read(struct bluetooth_inverter *inv,
		struct smadata2_l1_packet *p,struct smadata2_l2_packet *p2);

void in_smadata2plus_level1_packet_send(struct bluetooth_inverter *inv,
		struct smadata2_l1_packet *p);

void in_smadata2plus_level2_tryfcs16(unsigned char * buffer, int len, unsigned char * cs);

void in_smadata2plus_level2_packet_read(unsigned char *buffer, int len,
		struct smadata2_l2_packet *p);

int in_smadata2plus_level2_packet_gen(struct bluetooth_inverter *inv,
		unsigned char * buffer, struct smadata2_l2_packet *p);

void in_smadata2plus_level2_add_escapes(unsigned char *buffer, int *len);

void in_smadata2plus_level2_strip_escapes(unsigned char *buffer, int *len);

void in_smadata2plus_connect(struct bluetooth_inverter * inv);

void in_smadata2plus_login(struct bluetooth_inverter * inv);

void in_smadata2plus_get_values(struct bluetooth_inverter * inv);

#endif /* OPENSUNNY_IN_SMADATA2PLUS_H_ */
