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

#define SMADATA2PLUS_L2_INIT_FCS16 0xffff		// Initial FCS value

#define SMADATA2PLUS_MAX_VALUES 64

#include "in_smadata2plus_structs.h"


void in_smadata2plus_level1_clear(struct smadata2_l1_packet *p);

void in_smadata2plus_level1_cmdcode_wait(struct bluetooth_inverter * inv,
		struct smadata2_l1_packet *p, struct smadata2_l2_packet * p2 , int cmdcode);

void in_smadata2plus_level1_packet_print(char * output,
		struct smadata2_l1_packet *p);

int in_smadata2plus_level1_packet_read(struct bluetooth_inverter *inv,
		struct smadata2_l1_packet *p,struct smadata2_l2_packet *p2);

void in_smadata2plus_level1_packet_send(struct bluetooth_inverter *inv,
		struct smadata2_l1_packet *p);

void in_smadata2plus_level2_tryfcs16(unsigned char * buffer, int len, unsigned char * cs);

void in_smadata2plus_level2_packet_print(char * output,
		struct smadata2_l2_packet *p);

void in_smadata2plus_level2_packet_read(unsigned char *buffer, int len,
		struct smadata2_l2_packet *p);

int in_smadata2plus_level2_packet_gen(struct bluetooth_inverter *inv,
		unsigned char * buffer, struct smadata2_l2_packet *p);


void in_smadata2plus_level2_add_escapes(unsigned char *buffer, int *len);

void in_smadata2plus_level2_strip_escapes(unsigned char *buffer, int *len);

void in_smadata2plus_connect(struct bluetooth_inverter * inv);

void in_smadata2plus_login(struct bluetooth_inverter * inv);

void in_smadata2plus_get_historic_values(struct bluetooth_inverter * inv);

void in_smadata2plus_get_model(struct bluetooth_inverter * inv,unsigned char *model_code) ;

void in_smadata2plus_get_values(struct bluetooth_inverter * inv);

#endif /* OPENSUNNY_IN_SMADATA2PLUS_H_ */
