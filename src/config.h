#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "buffer.h"

#define CONFIG__RED            31
#define CONFIG__GREEN          32
#define CONFIG__YELLOW         33
#define CONFIG__BLUE           34
#define CONFIG__MAGENTA        35
#define CONFIG__CYAN           36
#define CONFIG__WHITE          37
#define CONFIG__BRIGHT_BLACK   90
#define CONFIG__BRIGHT_RED     91
#define CONFIG__BRIGHT_GREEN   92
#define CONFIG__BRIGHT_YELLOW  93
#define CONFIG__BRIGHT_BLUE    94
#define CONFIG__BRIGHT_MAGENTA 95
#define CONFIG__BRIGHT_CYAN    96
#define CONFIG__BRIGHT_WHITE   97

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

    int32_t _padding;
}
Conf_t;

// Opens the "~/.config/fiflorc"
bool config__load(Conf_t*);

// Sets the selectors names.
void config__init_selectors(Conf_t*);

// Check if selector_in_file == expected_selector.
bool config__parse_selector(Conf_t*, const char* const, const int);

// Convert a word into an ASCII escape code.
int config__parse_value(const char* const);

// As in the name. Wow.
void config__set_default(Conf_t*);

// As above but read from the file.
void config__load_custom(Conf_t*);

#endif
