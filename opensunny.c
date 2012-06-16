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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "opensunny.h"

char arg_inverter_mac[20];


/*
 * TODO: Configfile with ini and multi inverter support
 * TODO: Mode for analyzing sniff wireshark binfiles from  Sunny Explorer
 * TODO: Get historic data from inverter
 * TODO: DB Api for value storage
 * TODO: Autodetect Inverters
 */

void print_help(){

	printf("./opensunny [OPTIONS] -i [INVERTER_MAC]\n\n");
	printf("Options \n");
	printf("\t-v\tBe more verbose (repeat for even more verbosity)\n");


}

int parse_args(int argc, char **argv) {

	int arg_verbosity = 0;

	int count;

	if (argc > 1) {
		for (count = 1; count < argc; count++) {
			log_debug("Argument received argv[%d] = %s", count, argv[count]);

			if (strcmp(argv[count],"-v")==0){
				arg_verbosity++;
			} else if (strcmp(argv[count],"-i")==0){
				count++;
				strncpy(arg_inverter_mac,argv[count],19);
			}
		}

		if (arg_verbosity ==  1){
			logging_set_loglevel(logger,ll_verbose);
		} else if (arg_verbosity == 2) {
			logging_set_loglevel(logger,ll_debug);
		}

		if (strlen(arg_inverter_mac) != 17){
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

	/* Parsing Args */
	parse_args(argc,argv);

	/* Inizialize Bluetooth Inverter */
	struct bluetooth_inverter inv = { { 0 } };

	strcpy(inv.macaddr, arg_inverter_mac);

	memcpy(inv.password, "0000", 5);

	in_bluetooth_connect(&inv);

	in_smadata2plus_connect(&inv);

	in_smadata2plus_login(&inv);

	in_smadata2plus_get_values(&inv);

	close(inv.socket_fd);

	exit(0);
}
