#include "config.h"

void config__init_selectors(Conf_t* Config)
{
	strcpy(Config->Color_line_number_current.selector,
	       "color-line-number-current");

	strcpy(Config->Color_line_number.selector, "color-line-number");
	strcpy(Config->Color_ui.selector,          "color-ui");
	strcpy(Config->Color_text.selector,        "color-text");
	strcpy(Config->Color_warning.selector,     "color-warning");
	strcpy(Config->Tab_width.selector,         "tab-width");
}

void config__parse_selector(Conf_t* Config, char* selector, int value)
{
	if(!strcmp(Config->Color_ui.selector, selector))
	{
		Config->Color_ui.value = value;
	}
	if(!strcmp(Config->Color_line_number_current.selector, selector))
	{
		Config->Color_line_number_current.value = value;
	}
	else if(!strcmp(Config->Color_line_number.selector, selector))
	{
		Config->Color_line_number.value = value;
	}
	else if(!strcmp(Config->Color_text.selector, selector))
	{
		Config->Color_text.value = value;
	}
	else if(!strcmp(Config->Color_warning.selector, selector))
	{
		Config->Color_warning.value = value;
	}
	else if(!strcmp(Config->Tab_width.selector, selector))
	{
		Config->Tab_width.value = value;
	}
}

int config__encode_color(Conf_t* Config, char* color_name)
{
	Colors_t value = WHITE; // The default one.

	if(!strcmp(color_name, "red\n"))
	{
		value = RED;
	}
	else if(!strcmp(color_name, "green\n"))
	{
		value = GREEN;
	}
	else if(!strcmp(color_name, "yellow\n"))
	{
		value = YELLOW;
	}
	else if(!strcmp(color_name, "blue\n"))
	{
		value = BLUE;
	}
	else if(!strcmp(color_name, "magenta\n"))
	{
		value = MAGENTA;
	}
	else if(!strcmp(color_name, "cyan\n"))
	{
		value = CYAN;
	}
	else if(!strcmp(color_name, "white\n"))
	{
		value = WHITE;
	}
	else if(!strcmp(color_name, "bright-red\n"))
	{
		value = BRIGHT_RED;
	}
	else if(!strcmp(color_name, "bright-green\n"))
	{
		value = BRIGHT_GREEN;
	}
	else if(!strcmp(color_name, "bright-yellow\n"))
	{
		value = BRIGHT_YELLOW;
	}
	else if(!strcmp(color_name, "bright-blue\n"))
	{
		value = BRIGHT_BLUE;
	}
	else if(!strcmp(color_name, "bright-magenta\n"))
	{
		value = BRIGHT_MAGENTA;
	}
	else if(!strcmp(color_name, "bright-cyan\n"))
	{
		value = BRIGHT_CYAN;
	}
	else if(!strcmp(color_name, "bright-white\n"))
	{
		value = BRIGHT_WHITE;
	}
	else if(!strcmp(color_name, "2\n") || !strcmp(color_name, "4\n")
	        || !strcmp(color_name, "8\n"))
	{
		color_name[strlen(color_name) - 1] = '\0';
		value = atoi(color_name);
	}
	else
	{
		config__set_default(Config);
	}
	return value;
}

void config__set_default(Conf_t* Config)
{
	Config->Color_ui.value                  = WHITE;
	Config->Color_line_number.value         = BRIGHT_MAGENTA;
	Config->Color_line_number_current.value = WHITE;
	Config->Color_text.value                = GREEN;
	Config->Color_warning.value             = RED;
	Config->Tab_width.value                 = 4;
}

void config__set_custom(Conf_t* Config)
{
	int  parsed_value = 0;
	char line[128];
	char selector[48];
	char value[32];

	config__init_selectors(Config);

	while(fgets(line, 80, Config->File) != NULL)
	{
		if((line[0] == '#') || (line[0] <= 32))
		{
			continue;
		}
		// Splits around the " = ".
		strncpy(selector, strtok(line, " = "), 48);
		strncpy(value, strtok(NULL, " = "), 32);

		parsed_value = config__encode_color(Config, value);
		config__parse_selector(Config, selector, parsed_value);
	}
	if(parsed_value == 0) // If the whole file is commented out.
	{
		config__set_default(Config);
	}
}

void config__set_color(Opt_t* Option)
{
	if(Option == NULL)
	{
		printf("\033[0m"); // Reset colors and others to default.
	}
	else
	{
		printf("\033[%um", Option->value);
	}
}
