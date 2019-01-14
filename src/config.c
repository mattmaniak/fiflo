#include "config.h"

void config_set_default(Conf_t* Config)
{
	strcpy(Config->color_bar, WHITE);
	strcpy(Config->color_text, GREEN);
	strcpy(Config->color_line_number, MAGENTA_BRIGHT);
	strcpy(Config->color_current_line_number, WHITE);
	strcpy(Config->color_warning, RED);
}

bool config_set_custom(Conf_t* Config)
{
	char option[80];
	int  value;

	while(!feof(Config->file))
	{
		if(fscanf(Config->file, "%s %d", option, &value) == EOF)
		{
			config_set_default(Config);
			return false;
		}
		if(option[0] == '#')
		{
			continue;
		}
		// TODO: PARSING.
		// printf("%s %d\n", option, value);
	}
	return true;
}
