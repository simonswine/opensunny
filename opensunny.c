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


/*
 * TODO: Configfile with ini and multi inverter support
 * TODO: Mode for analyzing sniff wireshark binfiles from  Sunny Explorer
 * TODO: Get historic data from inverter
 * TODO: DB Api for value storage
 * TODO: Autodetect Inverters
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>

#include "opensunny.h"

char arg_inverter_mac[20];
dictionary * conf;
struct bluetooth_inverter inverters[MAX_INVERTERS];
int inverter_count = 0;

/*
 * Define Arguments
 */

static const struct option argv_parameters[] = {
	{ "help",		no_argument,		0,	'h' },
	{ "quiet",		no_argument,		0,	'q' },
	{ "verbose",	no_argument,		0,	'v' },
	{ "config",		required_argument,	0,	'c' },
	{ "mode",		required_argument,	0, 	'm' },
	{ "inverter",	required_argument,		0,	'i'	},
};

static const char *argv_help[] = {
	"Show help",
	"Be more quiet, repeatable",
	"Be more verbose, repeatable",
	"Config file",
	"Choose mode of opensunny" ,
	"Define inverter",
};


void print_help() {

	char mode[128];
	int mode_max_len=0;

	fprintf(stderr,"OpenSunny HELP\n");


	/* Run two times to find max */
	int run = 0;
	while (run < 2){
		for (int arg_pos = 0; arg_pos < (sizeof(argv_parameters)/sizeof(struct option)); ++arg_pos) {
			if (argv_parameters[arg_pos].has_arg == no_argument){
				strncpy(mode,argv_parameters[arg_pos].name,sizeof(mode)-1);
			} else {
				snprintf(mode,sizeof(mode)-1,"%s=<%s>",argv_parameters[arg_pos].name,argv_parameters[arg_pos].name);
				int i;
				for (i = strlen(argv_parameters[arg_pos].name)+2; i < (2*strlen(argv_parameters[arg_pos].name)+2); ++i)
					mode[i]=toupper(mode[i]);
			}
			if (strlen(mode) > mode_max_len)
				mode_max_len = strlen(mode);

			if (run >0)
				fprintf(stderr,"  -%c, --%-*s %s \n", argv_parameters[arg_pos].val,mode_max_len+1,mode, argv_help[arg_pos]);

		}
		run++;
	}


}

void default_config() {

}

void populate_inverter_list(dictionary * d, struct bluetooth_inverter invs[],
		int * inv_count, int inv_max) {

	/* Get max sections */
	int sec_count = iniparser_getnsec(d);

	/* Loop through sections */
	int sec_pos = 0;
	char *sec_name, *inv_mac, *inv_password, *inv_protocol, *inv_type;
	char key[128];
	char *sec_needle = "inverter_";
	for (sec_pos = 0; sec_pos < sec_count; ++sec_pos) {

		/* Check if Inverter Section */
		sec_name = iniparser_getsecname(d, sec_pos);
		if (strstr(sec_name, sec_needle) == sec_name) {

			/* inverter name to struct */
			strncpy(invs[*inv_count].name, sec_name + strlen(sec_needle),
					sizeof(invs[*inv_count].name) - 1);

			/* type check */
			snprintf(key, sizeof(key) - 1, "%s:%s", sec_name, "type");
			inv_type = iniparser_getstring(d, key, "null");
			snprintf(key, sizeof(key) - 1, "%s:%s", sec_name, "protocol");
			inv_protocol = iniparser_getstring(d, key, "null");
			if (strcasecmp(inv_type, "bluetooth") == 0) {

				/* Check valid protocols */
				if (strcasecmp(inv_protocol, "smadata2plus") == 0) {

					/* validate password */
					snprintf(key, sizeof(key) - 1, "%s:%s", sec_name,
							"password");
					inv_password = iniparser_getstring(d, key, "0000");
					if (strlen(inv_password) < 1) {
						log_error(
								"[Config][%s] Passwort too short: '%s'", sec_name, inv_password);
						break;
					} else if (strlen(inv_password) > 12) {
						log_error(
								"[Config][%s] Passwort too long: '%s'", sec_name, inv_password);
						break;
					} else {
						memcpy(invs[*inv_count].password, inv_password, 12);
					}

					/* validate macaddress */
					snprintf(key, sizeof(key) - 1, "%s:%s", sec_name,
							"bt_address");
					inv_mac = iniparser_getstring(d, key, "null");
					if (str_mac_validate(inv_mac) == 0) {
						log_error(
								"[Config][%s] Wrong Mac Address: '%s'", sec_name, iniparser_getstring(d,key,"null"));
						break;
					}
					strcpy(invs[*inv_count].macaddr, inv_mac);

					/* Log inverter found */
					log_info(
							"[Config][%s] Found type='%s' protocol='%s' mac='%s' password='%s'", sec_name,inv_type,inv_protocol,invs[*inv_count].macaddr,invs[*inv_count].password);

				} else {
					log_error(
							"[Config][%s] Invalid protocol '%s' for type '%s'", sec_name, inv_protocol, inv_type);
					break;
				}

			} else {
				log_error("[Config][%s] Unkown type: '%s'", sec_name, inv_type);
				break;
			}

		}

	}

}

void parse_config() {

	conf = iniparser_load("opensunny.ini");
	//dictionary_dump(conf, stderr);

	populate_inverter_list(conf, inverters, &inverter_count, MAX_INVERTERS);

}

int parse_args(int argc, char **argv) {

	int arg_verbosity = 0;

	int count;

	if (argc > 1) {
		for (count = 1; count < argc; count++) {
			log_debug("Argument received argv[%d] = %s", count, argv[count]);

			if (strcmp(argv[count], "-v") == 0) {
				arg_verbosity++;
			} else if (strcmp(argv[count], "-i") == 0) {
				count++;
				strncpy(arg_inverter_mac, argv[count], 19);
			}
		}

		if (arg_verbosity == 1) {
			logging_set_loglevel(logger, ll_verbose);
		} else if (arg_verbosity == 2) {
			logging_set_loglevel(logger, ll_debug);
		}

		if (strlen(arg_inverter_mac) != 17) {
			printf("Wrong mac!\n\n");
			print_help();
			exit(EXIT_FAILURE);
		}

	} else {
		print_help();
		exit(EXIT_FAILURE);
	}

	return 0;

}

/* Main Routine smatool */
int main(int argc, char **argv) {

	/* Enable Logging */
	log_init();

	parse_config();

	print_help();

	exit(0);

	/* Parsing Args */
	parse_args(argc, argv);

	/* Inizialize Bluetooth Inverter */
	struct bluetooth_inverter inv = { { 0 } };

	strcpy(inv.macaddr, arg_inverter_mac);

	memcpy(inv.password, "0000", 5);

	in_bluetooth_connect(&inv);

	in_smadata2plus_connect(&inv);

	in_smadata2plus_login(&inv);

	in_smadata2plus_get_values(&inv);

//	in_smadata2plus_get_historic_values(&inv);

//	/* Packet Structs */
//	struct smadata2_l1_packet recv_pl1 = { 0 };
//	struct smadata2_l2_packet recv_pl2 = { 0 };
//
//	while (1)
//		in_smadata2plus_level1_packet_read(&inv,&recv_pl1,&recv_pl2);

	close(inv.socket_fd);

	exit(0);
}
