#ifndef FIFLO_H
#define FIFLO_H

// Some shit that starts the program.

#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

// memory.h
extern void chk_ptr(f_mtdt* Buff, void* ptr, const char* err_msg);

// keyboard.h
extern char getch(f_mtdt* Buff);

// file.h
extern f_mtdt* set_fname(f_mtdt* Buff, const char* passed);
extern f_mtdt* read_file(f_mtdt* Buff);

// keyboard.h
extern f_mtdt* parse_key(f_mtdt* Buff, char key);

// render.h
extern void flush_window(f_mtdt* Buff);
extern void render_window(f_mtdt* Buff);

// Initializes all Buff structure members.
f_mtdt* init_buffer(f_mtdt* Buff, const char* arg);

// Program parameters, eg. "--help".
void options(const char* arg);

// Some initializers and the main program loop.
noreturn void run(const char* arg);

int main(const int argc, const char** argv);

#endif
