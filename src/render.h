#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/ioctl.h>

int16_t windowSize(int8_t axis);
void clearFrame(void);
int16_t charBuffer(char key, int8_t chars);
void window(char key, int8_t chars, char filename[32]);

#endif

