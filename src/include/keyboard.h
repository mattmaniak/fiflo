#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdio.h>
#include <stdnoreturn.h>
#include <termios.h>
#include <unistd.h>

// file.h
extern f_mtdt* edit_fname(f_mtdt* Buff, char key);

// window.h
extern void flush_window(f_mtdt* Buff);

// memory.h
extern noreturn void free_all_exit(f_mtdt* Buff, const bool status);

// charmap.h
extern f_mtdt* key_action(f_mtdt* Buff, char key);

// seqmap.h
extern f_mtdt* recognize_sequence(f_mtdt* Buff, char sequence[8]);

// Reads one char wthout confirming by the key. Based on the termios.
char getch(f_mtdt* Buff);

// Saves the last pressed key to the temponary buffer and analyzes it.
f_mtdt* parse_key(f_mtdt* Buff, char key);

#endif
