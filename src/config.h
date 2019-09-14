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
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    BRIGHT_BLACK   = 90,
    BRIGHT_RED,
    BRIGHT_GREEN,
    BRIGHT_YELLOW,
    BRIGHT_BLUE,
    BRIGHT_MAGENTA,
    BRIGHT_CYAN,
    BRIGHT_WHITE
}
Config__color_t;

#define CONFIG__MIN_TAB_SZ 2
#define CONFIG__MAX_TAB_SZ 8

#define CONFIG__PUNCH_CARD_W 80
#define CONFIG__MIN_PUNCH_CARD_W 1
#define CONFIG__MAX_PUNCH_CARD_W 1024

#define CONFIG__SELECTOR_SZ 48

typedef struct
{
    char selector[CONFIG__SELECTOR_SZ];
    int  value;
}
Opt_t;

typedef struct
{
    FILE* File;
    Opt_t Color_actual_file;
    Opt_t Color_line_number;
    Opt_t Color_line_number_actual;
    Opt_t Color_text;
    Opt_t Color_ui;
    Opt_t Color_warning;
    Opt_t Color_whitespace;
    Opt_t Punch_card_width;
    Opt_t Tab_sz;

    const int32_t _padding;
}
Conf_t;

// Open and load the specified configuration file.
bool config__load(Conf_t* const);

// Set selectors names.
void config__init_selectors(Conf_t* const);

// Check if a selector_in_file is equal to an expected_selector or not.
bool config__parse_selector(Conf_t* const, const char* const, const int);

// Convert a word into an ASCII escape code.
int config__parse_value(const char* const);

// Apply a built-in default config ignoring any config files.
void config__set_default(Conf_t* const);

// As above but read from a file.
void config__load_custom(Conf_t* const);

#endif
