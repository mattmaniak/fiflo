#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

void sigstpHandler(int nothing);
//void emptyHandler(int nothing);
char unixGetch(void);

#endif

