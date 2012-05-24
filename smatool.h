/*
 * smatool header file
 */

// glibc may need this
#ifndef _XOPEN_SOURCE
  #define _XOPEN_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <string.h>
#include <math.h>
#ifndef __USE_XOPEN
  #define __USE_XOPEN
#endif
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <curl/curl.h>
#ifndef _GNU_SOURCE
  #define _GNU_SOURCE
#endif
#include <getopt.h>

// Beautify debugging
#define debug_printf(...) if( debug == 1 ) { printf("%s line %d: ", __FILE__, __LINE__); printf (__VA_ARGS__); }
#define verbose_printf(...) if( verbose == 1 ) { printf("Verbose: "); printf (__VA_ARGS__); }
