#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"
#include "logging.h"
#include "in_bluetooth.h"
#include "in_smadata2plus.h"

/* INVERTER Code */
unsigned char SMADATA2PLUS_INVCODE[6] = { 0x5c, 0xaf, 0xf0, 0x1d, 0x50, 0x00 };
//unsigned char SMADATA2PLUS_INVCODE[6] = { 0x78, 0x00, 0xd6, 0x12, 0x70, 0x39 };

/* L1 Stuff */
unsigned char SMADATA2PLUS_L1_CONTENT_BROADCAST[13] = { 0x00, 0x04, 0x70, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 };

/* L2 Stuff */
unsigned char SMADATA2PLUS_L2_HEADER[4] = { 0xFF, 0x03, 0x60, 0x65 };
unsigned char SMADATA2PLUS_L2_CONTENT_2[9] = { 0x80, 0x0E, 0x01, 0xFD, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF };

static u_int16_t SMADATA2PLUS_L2_FCSTAB[256] = { 0x0000, 0x1189, 0x2312, 0x329b,
		0x4624, 0x57ad, 0x6536, 0x74bf, 0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c,
		0xdbe5, 0xe97e, 0xf8f7, 0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c,
		0x75b7, 0x643e, 0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff,
		0xe876, 0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
		0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5, 0x3183,
		0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c, 0xbdcb, 0xac42,
		0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974, 0x4204, 0x538d, 0x6116,
		0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb, 0xce4c, 0xdfc5, 0xed5e, 0xfcd7,
		0x8868, 0x99e1, 0xab7a, 0xbaf3, 0x5285, 0x430c, 0x7197, 0x601e, 0x14a1,
		0x0528, 0x37b3, 0x263a, 0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960,
		0xbbfb, 0xaa72, 0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630,
		0x17b9, 0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
		0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738, 0xffcf,
		0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70, 0x8408, 0x9581,
		0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7, 0x0840, 0x19c9, 0x2b52,
		0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff, 0x9489, 0x8500, 0xb79b, 0xa612,
		0xd2ad, 0xc324, 0xf1bf, 0xe036, 0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5,
		0x4f6c, 0x7df7, 0x6c7e, 0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7,
		0xc03c, 0xd1b5, 0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74,
		0x5dfd, 0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
		0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c, 0xc60c,
		0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3, 0x4a44, 0x5bcd,
		0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb, 0xd68d, 0xc704, 0xf59f,
		0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232, 0x5ac5, 0x4b4c, 0x79d7, 0x685e,
		0x1ce1, 0x0d68, 0x3ff3, 0x2e7a, 0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a,
		0xb0a3, 0x8238, 0x93b1, 0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb,
		0x0e70, 0x1ff9, 0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9,
		0x8330, 0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78 };

/* Print level1 packet from its struct */
void in_smadata2plus_level1_packet_print(char * output,
		struct smadata2_l1_packet *p) {

	/* for output */
	char src_addr_hex[20], dest_addr_hex[20], content_hex[(p->length
			- SMADATA2PLUS_L1_HEADER_LEN) * 3];
	buffer_hex_dump(src_addr_hex, p->src, 6);
	buffer_hex_dump(dest_addr_hex, p->dest, 6);
	buffer_hex_dump(content_hex, p->content,
			p->length - SMADATA2PLUS_L1_HEADER_LEN);

	sprintf(output, "length=%d cmdcode=%d src=%s dest=%s content=%s", p->length,
			p->cmd_code, src_addr_hex, dest_addr_hex, content_hex);

}

void in_smadata2plus_level2_packet_print(char * output,
		struct smadata2_l2_packet *p) {

	/* for output */
	char content_hex[p->content_length*3];
	buffer_hex_dump(content_hex, p->content, p->content_length);

	sprintf(output,
			"ctrl1=%02x ctrl2=%02x archcd=%02x zero=%02x c=%02x content[%dbytes]=%s",
			p->ctrl1, p->ctrl2, p->archcd, p->zero, p->c, p->content_length,
			content_hex);

}

/* Read level1 packet from stream */
int in_smadata2plus_level1_packet_read(struct bluetooth_inverter *inv,
		struct smadata2_l1_packet *p) {

	/* wait for start package */
	while (in_bluetooth_get_byte(inv) != SMADATA2PLUS_STARTBYTE) {
		usleep(500);
	}

	/* Fetching Checksum */
	unsigned char len1 = in_bluetooth_get_byte(inv);
	unsigned char len2 = in_bluetooth_get_byte(inv);
	p->checksum = in_bluetooth_get_byte(inv);
	unsigned char checksumvalidate = SMADATA2PLUS_STARTBYTE ^ len1 ^ len2;
	if (p->checksum != checksumvalidate) {
		log_debug("[L1] Received packet with wrong Checksum");
	}

	/* Fetching source + dest addresses */
	in_bluetooth_get_bytes(inv, p->src, 6);
	in_bluetooth_get_bytes(inv, p->dest, 6);

	/* reverse byte order */
	buffer_reverse(p->src, 6);
	buffer_reverse(p->dest, 6);

	/* cmdcode */
	p->cmd_code = in_bluetooth_get_byte(inv) + in_bluetooth_get_byte(inv) * 256;

	/* packet_len */
	p->length = len1 + (len2 * 256);

	/* getcontent */
	in_bluetooth_get_bytes(inv, p->content,
			p->length - SMADATA2PLUS_L1_HEADER_LEN);

	/* Packet pritn */
	char output[BUFSIZ];
	in_smadata2plus_level1_packet_print(output, p);

	log_debug("[L1] Received packet with %s", output);

	/* Check if contains L2 packet */
	if (p->length - SMADATA2PLUS_L1_HEADER_LEN > 4
			&& p->content[0] == SMADATA2PLUS_STARTBYTE
			&& memcmp(p->content + 1, SMADATA2PLUS_L2_HEADER, 4) == 0) {

		struct smadata2_l2_packet pl2 = {0};
		in_smadata2plus_level2_packet_read(p->content,
				p->length - SMADATA2PLUS_L1_HEADER_LEN, &pl2);

	}

	return p->cmd_code;

}

void in_smadata2plus_level2_packet_read(unsigned char *buffer, int len,
		struct smadata2_l2_packet *p) {

	int pos=0;

	/* Strip escapes */
	int len_bef = len;
	in_smadata2plus_level2_strip_escapes(buffer, &len);
	int diff = len_bef- len;

	/* Remove checksum */
	len -= 1;
	unsigned char checksum[2], checksum_recv[2];
	checksum_recv[1] = buffer[(len--)-1];
	checksum_recv[0] = buffer[(len--)-1];

	/* Generate checksum */
	in_smadata2plus_tryfcs16(buffer + 1, len-1, checksum);

	/* Log */
	log_debug("[L2] Unescaped %d chars, checksum %02x:%02x==%02x:%02x  ", diff, checksum[0], checksum[1],checksum_recv[0], checksum_recv[1]);

	/* Compare checksums */
	if (memcmp(checksum, checksum_recv, 2) != 0) {
		log_info("[L2] Received packet with wrong Checksum");
	}

	/*** Start Reading of packet ***/

	/* Start byte */
	pos++;

	/* header bytes */
	pos += sizeof(SMADATA2PLUS_L2_HEADER);

	/* Ctrl codes */
	p->ctrl1 = buffer[pos++];
	p->ctrl2 = buffer[pos++];

	/* Address style */
	pos += 6;

	/* ArchCd and zero */
	p->archcd = buffer[pos++];
 	p->zero = buffer[pos++];

	/* Address style again (invertercode) */
	pos += 6;

	/* zero and  c */
	pos++;
	p->c = buffer[pos++];

	/* four zeros */
	pos += 4;

	/* packetcount */
	pos += 2;

	/* content */
	p-> content_length = len - pos;
    memcpy(p->content,buffer+pos,p->content_length);

	/* Packet print */
	char output[BUFSIZ];
	in_smadata2plus_level2_packet_print(output, p);
	log_debug("[L2] Received packet with  %s", output);

}

int in_smadata2plus_level2_packet_gen(struct bluetooth_inverter *inv,
		unsigned char * buffer, struct smadata2_l2_packet *p) {

	/* Packet print */
	char output[BUFSIZ];
	in_smadata2plus_level2_packet_print(output, p);
	log_debug("[L2] Send packet with %s", output);

	/** Packet Header **/

	/* Length of buffer used */
	int len = 0;

	/* Startbyte */
	buffer[len++] = SMADATA2PLUS_STARTBYTE;

	/* Headerbytes */
	memcpy(buffer + len, SMADATA2PLUS_L2_HEADER,
			sizeof(SMADATA2PLUS_L2_HEADER));
	len += sizeof(SMADATA2PLUS_L2_HEADER);

	/* Ctrl codes */
	buffer[len++] = p->ctrl1;
	buffer[len++] = p->ctrl2;

	/* six 0xffs */
	buffer_repeat(buffer + len, 0xff, 6);
	len += 6;

	/* ArchCd and zero */
	buffer[len++] = p->archcd;
	buffer[len++] = p->zero;

	/* Inverter Code */
	memcpy(buffer + len, SMADATA2PLUS_INVCODE, sizeof(SMADATA2PLUS_INVCODE));
	len += sizeof(SMADATA2PLUS_INVCODE);

	/* zero and  c */
	buffer[len++] = 0x00;
	buffer[len++] = p->c;

	/* four zeros */
	buffer_repeat(buffer + len, 0x00, 4);
	len += 4;

	/* packetcount */
	buffer[len++] = inv->l2_packet_send_count;

	/* adding content */
	memcpy(buffer + len, p->content, p->content_length);
	len += p->content_length;

	/* build checksum of content */
	unsigned char checksum[2];
	in_smadata2plus_tryfcs16(buffer + 1, len - 1, checksum);

	/* Escape special chars */
	int len_bef = len;
	in_smadata2plus_level2_add_escapes(buffer, &len);

	/* Adding checksum */
	int checksum_len = 2;
	memcpy(buffer + len, checksum, 2);
	/* Escaping checksum if needed */
	in_smadata2plus_level2_add_escapes(buffer+len, &checksum_len);
	len += checksum_len;

	log_debug(
			"[L2] Escaped %d chars, checksum %02x:%02x", len-len_bef, checksum[0], checksum[1]);



	/* Trailing Byte */
	buffer[len++] = SMADATA2PLUS_STARTBYTE;

	return len;

}

void in_smadata2plus_level1_packet_send(struct bluetooth_inverter *inv,
		struct smadata2_l1_packet *p) {

	unsigned char buffer[BUFSIZ];
	int i = 0;

	/* Generate lengths and checksum */
	int len2 = (p->length / 256);
	int len1 = p->length - len2;
	p->checksum = SMADATA2PLUS_STARTBYTE ^ len1 ^ len2;

	/* Packet print */
	char output[BUFSIZ];
	in_smadata2plus_level1_packet_print(output, p);

	log_debug("[L1] Send packet with %s", output);

	/* Reverse Macs */
	buffer_reverse(p->src, 6);
	buffer_reverse(p->dest, 6);

	/* Command */
	int cmd2 = (p->cmd_code / 256);
	int cmd1 = p->cmd_code - cmd2;

	/* Build Packet */
	buffer[i++] = SMADATA2PLUS_STARTBYTE;
	buffer[i++] = len1;
	buffer[i++] = len2;
	buffer[i++] = p->checksum;
	memcpy(buffer + i, p->src, 6);
	i += 6;
	memcpy(buffer + i, p->dest, 6);
	i += 6;
	buffer[i++] = cmd1;
	buffer[i++] = cmd2;
	memcpy(buffer + i, p->content, p->length - SMADATA2PLUS_L1_HEADER_LEN);
	i += p->length - SMADATA2PLUS_L1_HEADER_LEN;

	in_bluetooth_write(inv, buffer, i);

}

void in_smadata2plus_level1_cmdcode_wait(struct bluetooth_inverter * inv,
		struct smadata2_l1_packet *p, int cmdcode) {

	log_debug("[L1] Wait for packet cmdcode == %d", cmdcode);
	int act_cmdcode = in_smadata2plus_level1_packet_read(inv, p);
	while (act_cmdcode != cmdcode) {
		act_cmdcode = in_smadata2plus_level1_packet_read(inv, p);
	}
	log_debug("[L1] Got packet cmdcode == %d", cmdcode);

}

void in_smadata2plus_level1_clear(struct smadata2_l1_packet *p) {
	memset(p, 0, sizeof(p));
}
void in_smadata2plus_level2_clear(struct smadata2_l2_packet *p) {
	memset(p, 0, sizeof(p));
}

void in_smadata2plus_level2_trailer(unsigned char * buffer,
		struct smadata2_l2_packet *p) {

//	  FCSChecksum =FCSChecksum ^ 0xffff;
//
//	  btbuffer[packetposition++]=FCSChecksum & 0x00ff;
//	  btbuffer[packetposition++]=(FCSChecksum >> 8) & 0x00ff;
//	  btbuffer[packetposition++]=0x7e;  //Trailing byte

}

void in_smadata2plus_connect(struct bluetooth_inverter * inv) {

	/* Intizalize packet structs */
	struct smadata2_l1_packet recv_pl1 = { 0 };
	struct smadata2_l1_packet sent_pl1 = { 0 };
	struct smadata2_l2_packet sent_pl2 = { 0 };

	/* Wait for Broadcast request */
	in_smadata2plus_level1_cmdcode_wait(inv, &recv_pl1,
			SMADATA2PLUS_L1_CMDCODE_BROADCAST);

	/* fetch netid from package */
	unsigned char netid = recv_pl1.content[4];

	/* Answer broadcast */
	sent_pl1.cmd_code = SMADATA2PLUS_L1_CMDCODE_BROADCAST;
	/* Set destination */
	memcpy(sent_pl1.dest, recv_pl1.src, 6);
	/* Set my address */
	in_bluetooth_get_my_address(inv, sent_pl1.src);

	int len = 0;
	/* Copy content for Broadcast */
	memcpy(sent_pl1.content, SMADATA2PLUS_L1_CONTENT_BROADCAST,
			sizeof(SMADATA2PLUS_L1_CONTENT_BROADCAST));
	len += sizeof(SMADATA2PLUS_L1_CONTENT_BROADCAST);
	/* Set netid */
	sent_pl1.content[4] = netid;

	/* Setting length of packet */
	sent_pl1.length = SMADATA2PLUS_L1_HEADER_LEN + len;

	/* Send Packet out */
	in_smadata2plus_level1_packet_send(inv, &sent_pl1);

	/* Wait for cmdcode 10 */
	in_smadata2plus_level1_cmdcode_wait(inv, &recv_pl1,
			SMADATA2PLUS_L1_CMDCODE_10);

	/* Wait for cmdcode 5 */
	in_smadata2plus_level1_cmdcode_wait(inv, &recv_pl1,
			SMADATA2PLUS_L1_CMDCODE_5);

	/** Sent first L2 packet*/
	in_smadata2plus_level1_clear(&sent_pl1);
	in_smadata2plus_level2_clear(&sent_pl2);
	/* Set cmdcode */
	sent_pl1.cmd_code = SMADATA2PLUS_L1_CMDCODE_LEVEL2;
	/* Set destination */
	buffer_repeat(sent_pl1.dest, 0xff, 6);
	/* Set my address */
	in_bluetooth_get_my_address(inv, sent_pl1.src);
	/* Set Layer 2 */
	sent_pl2.ctrl1 = 0x09;
	sent_pl2.ctrl2 = 0xa0;
	/* Set L2 Content */
	unsigned char content_packet_one[13] = { 0x80, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	memcpy(sent_pl2.content, content_packet_one, sizeof(content_packet_one));
	sent_pl2.content_length = sizeof(content_packet_one);
	/* Generate L2 Paket */
	memset(sent_pl1.content, 0, BUFSIZ);
	sent_pl1.length = in_smadata2plus_level2_packet_gen(inv, sent_pl1.content,
			&sent_pl2);
	sent_pl1.length += SMADATA2PLUS_L1_HEADER_LEN;
	/* Send Packet out */
	in_smadata2plus_level1_packet_send(inv, &sent_pl1);


	/* Wait for cmdcode 1 */
	in_smadata2plus_level1_cmdcode_wait(inv, &recv_pl1,
			SMADATA2PLUS_L1_CMDCODE_LEVEL2);


	/** Sent second L2 packet*/
	in_smadata2plus_level1_clear(&sent_pl1);
	in_smadata2plus_level2_clear(&sent_pl2);
	/* Set cmdcode */
	sent_pl1.cmd_code = SMADATA2PLUS_L1_CMDCODE_LEVEL2;
	/* Set destination */
	buffer_repeat(sent_pl1.dest, 0xff, 6);
	/* Set my address */
	in_bluetooth_get_my_address(inv, sent_pl1.src);
	/* Set Layer 2 */
	sent_pl2.ctrl1 = 0x08;
	sent_pl2.ctrl2 = 0xa0;
	/* Set L2 Content */
	unsigned char content_packet_two[9] = { 0x80, 0x0E, 0x01, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	memcpy(sent_pl2.content, content_packet_two, sizeof(content_packet_two));
	sent_pl2.content_length = sizeof(content_packet_two);
	/* Generate L2 Paket */
	sent_pl1.length = in_smadata2plus_level2_packet_gen(inv, sent_pl1.content,
			&sent_pl2);
	sent_pl1.length += SMADATA2PLUS_L1_HEADER_LEN;
	/* Send Packet out */
	in_smadata2plus_level1_packet_send(inv, &sent_pl1);

}

/*
 * How to use the fcs
 */
#define PPPINITFCS16 0xffff /* Initial FCS value    */

/*
 * Calculate a new fcs given the current fcs and the new data.
 */
u_int16_t in_smadata2plus_pppfcs16(u_int16_t fcs, void *_cp, int len) {
	register unsigned char *cp = (unsigned char *) _cp;

	while (len--)
		fcs = (fcs >> 8) ^ SMADATA2PLUS_L2_FCSTAB[(fcs ^ *cp++) & 0xff];
	return (fcs);
}

void in_smadata2plus_tryfcs16(unsigned char * buffer, int len,
		unsigned char * cs) {
	u_int16_t trialfcs;
	unsigned char stripped[BUFSIZ] = { 0 };

	memcpy(stripped, buffer, len);

	trialfcs = in_smadata2plus_pppfcs16(PPPINITFCS16, stripped, len);
	trialfcs ^= 0xffff; /* complement */

	cs[0] = (trialfcs & 0x00ff); /* least significant byte first */
	cs[1] = ((trialfcs >> 8) & 0x00ff);

}

void in_smadata2plus_level2_add_escapes(unsigned char *buffer, int *len) {
	int i, j;

	/* Loop through buffer from second byte*/
	for (i = 1; i < (*len); i++) {
		switch (buffer[i]) {

		/* chars to escape */
		case 0x7d:
		case 0x7e:
		case 0x11:
		case 0x12:
		case 0x13:

			/* move following chars */
			for (j = (*len); j > i; j--)
				buffer[j] = buffer[j - 1];

			/* add escape */
			buffer[i + 1] = buffer[i] ^ 0x20;
			buffer[i] = 0x7d;
			(*len)++;
			break;
		}
	}
}

void in_smadata2plus_level2_strip_escapes(unsigned char *buffer, int *len) {
	int i, j;

	for (i = 1; i < (*len); i++) {
		if (buffer[i] == 0x7d) {
			/* Found escape character. Need to convert*/
			buffer[i] = buffer[i + 1] ^ 0x20;

			/* Short buffer */
			for (j = i + 1; j < (*len) - 1; j++)
				buffer[j] = buffer[j + 1];
			(*len)--;
		}
	}
}
