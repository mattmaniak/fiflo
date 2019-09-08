#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "buffer.h"

typedef enum
{
    RED            = 31,
    GREEN          = 32,
    YELLOW         = 33,
    BLUE           = 34,
    MAGENTA        = 35,
    CYAN           = 36,
    WHITE          = 37,
    BRIGHT_BLACK   = 90,
    BRIGHT_RED     = 91,
    BRIGHT_GREEN   = 92,
    BRIGHT_YELLOW  = 93,
    BRIGHT_BLUE    = 94,
    BRIGHT_MAGENTA = 95,
    BRIGHT_CYAN    = 96,
    BRIGHT_WHITE   = 97
}
Config__color_t;

#define CONFIG__MIN_TAB_SZ 2
#define CONFIG__MAX_TAB_SZ 8

typedef struct
{
    char selector[32];
    int  value;
}
Opt_t;

typedef struct
{
    FILE* File;
    Opt_t Color_current_file;
    Opt_t Color_line_number;
    Opt_t Color_line_number_current;
    Opt_t Color_text;
    Opt_t Color_ui;
    Opt_t Color_warning;
    Opt_t Color_whitespace;
    Opt_t Tab_sz;
}
Conf_t;

// Open the "~/.config/fiflorc"
bool config__load(Conf_t*);

// Set selectors names.
void config__init_selectors(Conf_t*);

// Check if a selector_in_file == expected_selector.
bool config__parse_selector(Conf_t*, const char* const, const int);

// Convert a word into an ASCII escape code.
int config__parse_value(const char* const);

// As in the name.
void config__set_default(Conf_t*);

// As above but read from a file.
void config__load_custom(Conf_t*);

#endif
