#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/ioctl.h>

int16_t windowSize(int8_t axis);
void cleanFrame(void);
int8_t lineLenCheck(int8_t chars);
void window(char key, char filename[32], int8_t chars, int8_t lines);
void windowEmpty(char filename[32], int8_t chars, int8_t lines);

#endif

