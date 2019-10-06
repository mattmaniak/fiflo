#ifndef CONFIG_H
#define CONFIG_H

// Built-in configuration and a config file handling.

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ansi.h"
#include "v_file.h"

#define CONFIG__MIN_TAB_SZ 2
#define CONFIG__MAX_TAB_SZ 8

#define CONFIG__PUNCH_CARD_W     80
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
    Opt_t Color_txt;
    Opt_t Color_ui;
    Opt_t Color_whitespace;
    Opt_t Pcard_w;
    Opt_t Tab_sz;

    const int32_t _padding;
}
Config;

// Open and load the specified configuration file.
bool config__load(Config* const);

// Set selectors names.
void config__init_selectors(Config* const);

// Check if a selector_in_file is equal to an expected_selector or not.
bool config__parse_selector(Config* const, const char* const, const int);

// Convert a word into an ASCII escape code.
int config__parse_value(const char* const);

// Apply a built-in default config ignoring any config files.
void config__set_default(Config* const);

// As above but read from a file.
void config__load_custom(Config* const);

#endif
