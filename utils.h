#include "smatool.h"
#ifndef TRUE
  #define TRUE    1
#endif
#ifndef FALSE
  #define FALSE    1
#endif

//
void buffer_hex_dump(char * output, unsigned char * buffer, int len);

void buffer_reverse(unsigned char * buffer, int len);

void buffer_repeat(unsigned char * buffer, unsigned char c, int count);

unsigned char conv(char *nn);
//
int isSMA2plusPackage(unsigned char *package, int psize, int debug);
