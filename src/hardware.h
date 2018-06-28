#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

void pointerCheck(void *ptr, char *errmsg);
char unixGetch(void);

#endif

