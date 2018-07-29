#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>

#define MAX_CHARS_PER_LINE 80

#define READ 1
#define WRITE 0

#define X true
#define Y false

typedef uint16_t term_t;

term_t termSize(bool axis);
void cleanFrame(void);
void renderText(struct Data buff);
void windowFill(buff_t lines);
struct Data window(struct Data buff, char key);

#endif

