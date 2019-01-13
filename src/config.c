#include "config.h"

void config_set_default(Conf_t* Conf)
{
	strcpy(Conf->color_bar, WHITE);
	strcpy(Conf->color_text, GREEN);
	strcpy(Conf->color_line_number, MAGENTA_BRIGHT);
	strcpy(Conf->color_current_line_number, WHITE),
	strcpy(Conf->color_warning, RED);
}

bool config_set_custom(Conf_t* Conf)
{
	// TODO
	return true;
}
