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
/* level2 packet */
struct smadata2_value {
	char name[64];
	char unit[5];
	float factor;
	unsigned long value;
	int timestamp;
	unsigned char q_ctrl1;
	unsigned char q_ctrl2;
	unsigned char q_archcd;
	unsigned char q_zero;
	unsigned char q_c;
	unsigned char q_content[128];
	int q_content_length;
	unsigned char r_ctrl1;
	unsigned char r_ctrl2;
	int r_value_pos;
	int r_value_len;
	int r_timestamp_pos;
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

#endif /* IN_SMADATA2PLUS_H_ */
