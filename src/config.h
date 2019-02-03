#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
	RED            = 31,
	GREEN          = 32,
	YELLOW         = 33,
	BLUE           = 34,
	MAGENTA        = 35,
	CYAN           = 36,
	WHITE          = 37,
	BRIGHT_RED     = 91,
	BRIGHT_GREEN   = 92,
	BRIGHT_YELLOW  = 93,
	BRIGHT_BLUE    = 94,
	BRIGHT_MAGENTA = 95,
	BRIGHT_CYAN    = 96,
	BRIGHT_WHITE   = 97
}
Colors_t;

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
	Opt_t Color_text;
	Opt_t Color_warning;
	Opt_t Tab_width;
}
Conf_t;

//
void config__init_selectors(Conf_t*);

// Check if selector_in_file == expected_selector.
void config__parse_selector(Conf_t*, char*, int);

// Convert a word into an ASCII escape code.
int config__encode_color(Conf_t*, char*);

//
void config__set_default(Conf_t*);

//
void config__load_custom(Conf_t*);

//
void config__set_color(Opt_t*);

#endif
