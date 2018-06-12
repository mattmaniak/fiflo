#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

void pointerCheck(void* pointer);
char unixGetch(void);
void saveToFile(char base_filename[]);

#endif

