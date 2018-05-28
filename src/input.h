#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <unistd.h>
#include <termios.h>

int8_t unixGetch(void);
int8_t getKey(void);
#endif

