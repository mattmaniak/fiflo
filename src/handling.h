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
#define NEGATIVE_CHAR -1
#define NULLTERM 0
#define CTRL_D 4
#define TAB 9
#define LINEFEED 10
#define CTRL_X 24
#define SPACE 32
#define BACKSPACE 127

#define NULLTERM_SZ 1

void ptr_check(void* ptr, const char* errmsg); // Exit on error with message.
char nix_getch(void);

void set_fname(buff data, char* passed);

buff_t get_file_sz(FILE* fd);
buff read_file(buff data);
void save_file(buff data);

buff addition_char(buff data, char key);
buff keyboard_shortcut(buff data, char key);
void limits(buff data); // Prevent overflows
buff alloc_text(buff data, char key); // Place the char into the string.

#endif

