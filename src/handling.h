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
#define NULLTERM 0
#define CTRL_D 4
#define TAB 9
#define LF 10
#define CTRL_X 24
#define SPACE 32
#define BACKSPACE 127

#define NTERM_SZ 1

void ptr_check(void* ptr, const char* errmsg); // Exit on error with a message.
char nix_getch(void);

void set_fname(buff dat, char* passed);

buff read_file(buff dat);
void save_file(buff dat);

buff add_char(buff dat, char key);
buff keyboard_shortcut(buff dat, char key);
void limits(buff dat); // Prevent overflows
buff txt_alloc(buff dat, char key); // Place the char into the string.

#endif

