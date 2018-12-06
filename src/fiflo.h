#ifndef FIFLO_H
#define FIFLO_H

#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// memory.h
extern _Noreturn void free_buff_exit(f_mtdt* Buff, const bool status);

// file.h
extern f_mtdt* set_fname(f_mtdt* Buff, const char* passed);
extern f_mtdt* read_file(f_mtdt* Buff);

// text_processing.h
extern f_mtdt* parse_key(f_mtdt* Buff, char key);

// render.h
extern void flush_window (f_mtdt* Buff);
extern void render_window(f_mtdt* Buff);

// Initializes all Buff structure members.
f_mtdt* init_buffer(f_mtdt* Buff, const char* arg);

// Program parameters, eg. "--help".
void options(const char* arg);

// Reads one char wthout confirming by the key. Based on the termios.
char getch(f_mtdt* Buff);

// Some initializers and the main program loop.
_Noreturn void run(const char* arg);

int main(const int argc, const char** argv);

#endif
