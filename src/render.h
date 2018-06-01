#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/ioctl.h>

uint16_t windowSize(int8_t axis);
void clearFrame(void);
void window(char pressedKey, int8_t chars);

#endif

