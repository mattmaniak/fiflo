#ifndef HANDLING_H
#define HANDLING_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define READ true
#define WRITE false

#define MAX_PATH 4096
#define MAX_NAME 255

// Required keymap.
#define TERMINATOR 0
#define CTRL_D 4
#define LINEFEED 10
#define CTRL_X 24
#define ARROW_UP 65
#define ARROW_DOWN 66
#define ARROW_RIGHT 67
#define ARROW_LEFT 68
#define BACKSPACE 127

buff_t get_file_sz(FILE* fd);
char nix_getch(void);
void ptr_check(void* ptr, const char* errmsg);
void chars_limit(buff_t chars);

void set_filename(buff data, char* name);
buff read_file(buff data, char* name);
void save_file(buff data);

buff count_lines(buff data);

buff printable_key(buff data, char key);
buff shortcut(buff data, char key);
buff alloc_text(buff data, char key);

#endif

