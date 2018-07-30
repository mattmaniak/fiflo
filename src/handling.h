#ifndef HANDLING_H
#define HANDLING_H

#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define READ true
#define WRITE false

// Required keymap.
#define TERMINATOR 0
#define CTRL_D 4
#define LINEFEED 10
#define CTRL_X 24
#define BACKSPACE 127

buff_t get_file_sz(FILE *fd);
char nix_getch(void);
void chars_limit(buff_t chars);
void set_filename(buff data, char *name);
buff punched_card(buff data, term_t limit, bool mode, char key);
buff read_file(buff data, char *name);
void save_file(buff data);
buff alloc_text(buff data, char key);

#endif

