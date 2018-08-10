#ifndef HANDLING_H
#define HANDLING_H

#include <linux/limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include <termios.h>


// Required keymap.
#define NEGATIVE_CHAR -1
#define TERMINATOR 0
#define CTRL_D 4
#define TAB 9
#define LINEFEED 10
#define CTRL_X 24
#define ARROW_UP 65
#define ARROW_DOWN 66
#define ARROW_RIGHT 67
#define ARROW_LEFT 68
#define BACKSPACE 127

#define TERMINATOR_SZ 1

// Functions connected with the program API. Generally what's unseen.
void ptr_check(void* ptr, const char* errmsg); // Exit on error with message.
char nix_getch(void);

void set_fname(buff data, char* passed);

buff_t get_file_sz(FILE* fd);
buff read_file(buff data);
void save_file(buff data);

buff visible_char(buff data, char key);
buff keyboard_shortcut(buff data, char key);
buff count_lines(buff data);
void limits(buff data); // Prevent overflows
buff alloc_text(buff data, char key); // Place the char into the string.

#endif

