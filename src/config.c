#include "config.h"

bool config__load(Config* const config)
{
    const char conf_fname[] = "/etc/fiflorc";

    if(access(conf_fname, F_OK) == -1) // There is no config file.
    {
        config__set_default(config);
        return true;
    }
    config->file = fopen(conf_fname, "r");
    if(config->file != NULL)
    {
        config__load_custom(config);

        if(fclose(config->file) == EOF)
        {
            fprintf(stderr, "Unable to close a configuration file.\n");
            return false;
        }
    }
    return true;
}

void config__init_selectors(Config* const config)
{
    strcpy(config->color_txt.selector,          "color_txt");
    strcpy(config->color_ui.selector,           "color_ui");
    strcpy(config->color_whitespace.selector,   "color_whitespace");
    strcpy(config->punched_card_width.selector, "punch_card_width");
    strcpy(config->tab_sz.selector,             "tab_size");
}

bool config__parse_selector(Config* const config, const char* const selector,
                            const int value)
{
    // Adds a value to a found selector in a configuration structure.
    if(value >= RED)
    {
        if(!strcmp(config->color_ui.selector, selector))
        {
            config->color_ui.value = value;
        }
        else if(!strcmp(config->color_txt.selector, selector))
        {
            config->color_txt.value = value;
        }
        else if(!strcmp(config->color_whitespace.selector, selector))
        {
            config->color_whitespace.value = value;
        }
        else if(!strcmp(config->punched_card_width.selector, selector))
        {
            config->punched_card_width.value = value;
        }
    }
    else if(!strcmp(config->tab_sz.selector, selector))
    {
        if((value >= CONFIG__MIN_TAB_SZ) && (value <= CONFIG__MAX_TAB_SZ))
        {
            config->tab_sz.value = value;
        }
    }
    else
    {
        return false;
    }
    return true;
}

int config__parse_value(const char* const read_value)
{
    if(!strcmp(read_value, "red"))
    {
        return RED;
    }
    else if(!strcmp(read_value, "green"))
    {
        return GREEN;
    }
    else if(!strcmp(read_value, "yellow"))
    {
        return YELLOW;
    }
    else if(!strcmp(read_value, "blue"))
    {
        return BLUE;
    }
    else if(!strcmp(read_value, "magenta"))
    {
        return MAGENTA;
    }
    else if(!strcmp(read_value, "cyan"))
    {
        return CYAN;
    }
    else if(!strcmp(read_value, "white"))
    {
        return WHITE;
    }
    else if(!strcmp(read_value, "bright_black"))
    {
        return BRIGHT_BLACK;
    }
    else if(!strcmp(read_value, "bright_red"))
    {
        return BRIGHT_RED;
    }
    else if(!strcmp(read_value, "bright_green"))
    {
        return BRIGHT_GREEN;
    }
    else if(!strcmp(read_value, "bright_yellow"))
    {
        return BRIGHT_YELLOW;
    }
    else if(!strcmp(read_value, "bright_blue"))
    {
        return BRIGHT_BLUE;
    }
    else if(!strcmp(read_value, "bright_magenta"))
    {
        return BRIGHT_MAGENTA;
    }
    else if(!strcmp(read_value, "bright_cyan"))
    {
        return BRIGHT_CYAN;
    }
    return 0;
}

void config__set_default(Config* const config)
{
    config->color_txt.value          = WHITE;
    config->color_ui.value           = WHITE;
    config->color_whitespace.value   = BRIGHT_BLACK;
    config->punched_card_width.value = CONFIG__PUNCH_CARD_W;
    config->tab_sz.value             = CONFIG__MAX_TAB_SZ;
}

void config__load_custom(Config* const config)
{
    const char space_or_control_ch = 32;
    int        parsed_value        = 0;
    char       line[80];
    char       selector[CONFIG__SELECTOR_SZ];
    char       value[32];

    config__init_selectors(config);

    while(fgets(line, 80, config->file) != NULL)
    {
        if((line[0] == '#') || (line[0] <= space_or_control_ch))
        {
            continue;
        }
        // Split a string around the " = " into the "selector" and "value".
        strncpy(selector, strtok(line, " = "), CONFIG__SELECTOR_SZ);
        strncpy(value,    strtok(NULL, " = "), 32);

        parsed_value = config__parse_value(value);

        if(!config__parse_selector(config, selector, parsed_value))
        {
            config__set_default(config);
            break;
        }
    }
    if(parsed_value == 0) // If a whole file is commented out.
    {
        config__set_default(config);
    }
}
