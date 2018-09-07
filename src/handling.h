#ifndef HANDLING_H
#define HANDLING_H // Non-seen API things that makes text && misc. things works.

// Common rest is included in 'fiflo.h' There are only file-specific imports.
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include <termios.h>

// Required keymap.
#define NEG_CHAR -1
#define NTERM 0
#define CTRL_D 4
#define TAB 9
#define LF 10
#define CTRL_X 24
#define SPACE 32
#define BACKSPACE 127

#define INDEX 1
#define NTERM_SZ 1

void fnameset(buff dt, const char* passed);
buff readfile(buff dt);
void savefile(buff dt);

buff allocblk(buff dt, char mode);
void free_block(buff dt);
void freeall(buff dt);

buff add_char(buff dt, char key);
buff keyboard_shortcut(buff dt, char key);
buff alloc_chr(buff dt, char key);

void limits(buff dt); // Prevent overflows
buff handle_key(buff dt, char key); // Place the char into the string.

#endif

