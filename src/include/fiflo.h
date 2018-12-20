#ifndef FIFLO_H
#define FIFLO_H

// Some shit that starts the program.

#include <stdio.h>
#include <stdnoreturn.h>
#include <stdlib.h>

// memory.h
extern void chk_ptr(f_mtdt* Buff, void* ptr, const char* err_msg);

// keyboard.h
extern char getch(f_mtdt* Buff);

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
void run(const char* arg);

int main(const int argc, const char** argv);

static const struct
{
	f_mtdt* (*init_buffer)(f_mtdt* Buff, const char* arg);
	void (*options)(const char* arg);
	void (*run)(const char* arg);
}
fiflo =
{
	init_buffer,
	options,
	run
};

#endif
