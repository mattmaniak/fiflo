#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef enum
{
	BLACK          = 30,
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
Colors_t;

#define BRIGHT_RED 91
#define RESET 0

typedef struct
{
	char selector[32];
	int  value;
}
Opt_t;

typedef struct
{
	FILE* file;
	// Opt_t Color_bar;
	Opt_t Color_git_logo;
	// Opt_t Color_line_number;
	Opt_t Color_line_number_current;
	Opt_t Color_text;
	Opt_t Color_warning;
	Opt_t Tab_width;
}
Conf_t;

//
void config__init_selectors(Conf_t* Config);

// Check if selector_in_file == expected_selector.
void config__parse_selector(Conf_t* Config, char* selector, int value);

// Convert a word into an ASCII escape code.
int config__encode_color(Conf_t* Config, char* color_name);

//
void config__set_default(Conf_t* Conf);

//
void config__set_custom(Conf_t* Conf);

//
void config__set_color(Opt_t* Option);

#endif
