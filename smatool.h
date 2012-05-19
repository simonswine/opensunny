#define _XOPEN_SOURCE /* glibc needs this */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <string.h>
#include <math.h>
#define __USE_XOPEN
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <curl/curl.h>

// Beautify debugging
#define debug_printf(...) if( debug == 1 ) { printf("%s line %d: ", __FILE__, __LINE__); printf (__VA_ARGS__); }
