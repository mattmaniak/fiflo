#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

uint16_t windowSize(int8_t axis);
void clearWindow(void);
void window(int8_t pressedKey, int32_t charCount);
#endif

