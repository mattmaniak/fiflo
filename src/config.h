#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define RED            "\x1b[31m"
#define GREEN          "\x1b[32m"
#define YELLOW         "\x1b[33m"
#define BLUE           "\x1b[34m"
#define MAGENTA        "\x1b[35m"
#define CYAN           "\x1b[36m"
#define WHITE          "\x1b[37m"
#define RED_BRIGHT     "\x1b[91m"
#define GREEN_BRIGHT   "\x1b[92m"
#define YELLOW_BRIGHT  "\x1b[93m"
#define BLUE_BRIGHT    "\x1b[94m"
#define MAGENTA_BRIGHT "\x1b[95m"
#define CYAN_BRIGHT    "\x1b[96m"
#define WHITE_BRIGHT   "\x1b[97m"

typedef struct
{
	FILE*  file;
	char   color_bar[8];
	char   color_text[8];
	char   color_line_number[8];
	char   color_current_line_number[8];
	char   color_warning[8];
	// term_t punch_card;
}
Conf_t;

//
void config_set_default(Conf_t* Conf);

//
bool config_set_custom(Conf_t* Conf);

#endif
