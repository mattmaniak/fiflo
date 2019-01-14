#include "config.h"

void config_set_default(Conf_t* Config)
{
	strcpy(Config->color_bar, WHITE);
	strcpy(Config->color_text, GREEN);
	strcpy(Config->color_line_number, MAGENTA_BRIGHT);
	strcpy(Config->color_current_line_number, WHITE),
	strcpy(Config->color_warning, RED);
}

bool config_set_custom(Conf_t* Config)
{
	const int max_line_length = 80;
	char      line[max_line_length];

	while(!feof(Config->file))
	{
		if(fgets(line, max_line_length, Config->file) == NULL)
		{
			config_set_default(Config);
			return false;
		}
		// TODO: PARSING.
		// printf("%s", line);
	}
	return true;
}
