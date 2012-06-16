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


/*
 * TODO: Configfile with ini and multi inverter support
 * TODO: Mode for analyzing sniff wireshark binfiles from  Sunny Explorer
 * TODO: Get historic data from inverter
 * TODO: DB Api for value storage
 *
 */

/* Main Routine smatool */
int main(int argc, char **argv) {

	/* Enable Logging */
	log_init();

	/* Inizialize Bluetooth Inverter */
	struct bluetooth_inverter inv = { { 0 } };

	strcpy(inv.macaddr, "00:80:25:22:C6:3B");

	memcpy(inv.password, "0000", 5);

	in_bluetooth_connect(&inv);

	in_smadata2plus_connect(&inv);

	in_smadata2plus_login(&inv);

	in_smadata2plus_get_values(&inv);

	close(inv.socket_fd);

	exit(0);
}
