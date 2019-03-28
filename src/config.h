#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RED            31
#define GREEN          32
#define YELLOW         33
#define BLUE           34
#define MAGENTA        35
#define CYAN           36
#define WHITE          37
#define BRIGHT_BLACK   90
#define BRIGHT_RED     91
#define BRIGHT_GREEN   92
#define BRIGHT_YELLOW  93
#define BRIGHT_BLUE    94
#define BRIGHT_MAGENTA 95
#define BRIGHT_CYAN    96
#define BRIGHT_WHITE   97

typedef struct
{
    char selector[32];
    int  value;
}
Opt_t;

typedef struct
{
    FILE* File;
    Opt_t Color_ui;
    Opt_t Color_line_number;
    Opt_t Color_line_number_current;
    Opt_t Color_whitespace;
    Opt_t Color_text;
    Opt_t Color_warning;
    Opt_t Tab_width;
}
Conf_t;

// Opens the "~/.config/fiflorc"
bool config__load(Conf_t*);

// Sets the selectors names.
void config__init_selectors(Conf_t*);

// Check if selector_in_file == expected_selector.
void config__parse_selector(Conf_t*, char*, int);

// Convert a word into an ASCII escape code.
int config__encode_color(Conf_t*, char*);

// As in the name. Wow.
void config__set_default(Conf_t*);

// As above but read from the file.
void config__load_custom(Conf_t*);

#endif
