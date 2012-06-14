/*
 * in_smadata2plus.h
 *
 *  Created on: 08.06.2012
 *      Author: christian
 */

#ifndef IN_SMADATA2PLUS_H_
#define IN_SMADATA2PLUS_H_

#define SMADATA2PLUS_STARTBYTE 0x7e
#define SMADATA2PLUS_L1_HEADER_LEN 18

#define SMADATA2PLUS_L1_CMDCODE_LEVEL2 1		// 0x0001
#define SMADATA2PLUS_L1_CMDCODE_BROADCAST 2  	// 0x0002
#define SMADATA2PLUS_L1_CMDCODE_5 5  			// 0x0005
#define SMADATA2PLUS_L1_CMDCODE_10 10  			// 0x000a
#define SMADATA2PLUS_L1_CMDCODE_12 12  			// 0x000c
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

//unsigned char SMADATA2PLUS_L2_HEADER[4] = 0xFF036065;

int in_smadata2plus_level1_packet_read(struct bluetooth_inverter *inv,
		struct smadata2_l1_packet *p);

void in_smadata2plus_level1_packet_send(struct bluetooth_inverter *inv,
		struct smadata2_l1_packet *p);

void in_smadata2plus_tryfcs16(unsigned char * buffer, int len, unsigned char * cs);

void in_smadata2plus_level2_packet_read(unsigned char *buffer, int len,
		struct smadata2_l2_packet *p);

int in_smadata2plus_level2_packet_gen(struct bluetooth_inverter *inv,
		unsigned char * buffer, struct smadata2_l2_packet *p);

void in_smadata2plus_level2_add_escapes(unsigned char *buffer, int *len);

void in_smadata2plus_level2_strip_escapes(unsigned char *buffer, int *len);

#endif /* IN_SMADATA2PLUS_H_ */
