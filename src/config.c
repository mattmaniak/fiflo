#include "config.h"

bool config__load(Config* const config)
{
    const char conf_fname[] = "/etc/fiflorc";

    if(access(conf_fname, F_OK) == -1) // There is no config file.
    {
        config__set_default(config);
        return true;
    }
    config->File = fopen(conf_fname, "r");
    if(config->File != NULL)
    {
        config__load_custom(config);

        if(fclose(config->File) == EOF)
        {
            fprintf(stderr, "Unable to close a configuration file.\n");
            return false;
        }
    }
    return true;
}

void config__init_selectors(Config* const config)
{
    strcpy(config->Color_txt.selector,        "color_text");
    strcpy(config->Color_ui.selector,         "color_ui");
    strcpy(config->Color_whitespace.selector, "color_whitespace");
    strcpy(config->Pcard_w.selector,          "punch_card_width");
    strcpy(config->Tab_sz.selector,           "tab_size");
}

bool config__parse_selector(Config* const config, const char* const selector,
                            const int value)
{
    // Adds a value to a found selector in a configuration structure.
    if(value >= RED)
    {
        if(!strcmp(config->Color_ui.selector, selector))
        {
            config->Color_ui.value = value;
        }
        else if(!strcmp(config->Color_txt.selector, selector))
        {
            config->Color_txt.value = value;
        }
        else if(!strcmp(config->Color_whitespace.selector, selector))
        {
            config->Color_whitespace.value = value;
        }
        else if(!strcmp(config->Pcard_w.selector, selector))
        {
            config->Pcard_w.value = value;
        }
    }
    else if(!strcmp(config->Tab_sz.selector, selector))
    {
        if((value >= CONFIG__MIN_TAB_SZ) && (value <= CONFIG__MAX_TAB_SZ))
        {
            config->Tab_sz.value = value;
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
    else if(!strcmp(read_value, "bright_white"))
    {
        return BRIGHT_WHITE;
    }
    return 0;
}

void config__set_default(Config* const config)
{
    config->Color_txt.value         = WHITE;
    config->Color_ui.value          = WHITE;
    config->Color_whitespace.value  = BRIGHT_BLACK;
    // config->Pcard_w.value           = CONFIG__PUNCH_CARD_W;
    config->Pcard_w.value           = 48;
    config->Tab_sz.value            = CONFIG__MAX_TAB_SZ;
}

void config__load_custom(Config* const config)
{
    const char space_or_control_ch = 32;
    int        parsed_value        = 0;
    char       line[80];
    char       selector[CONFIG__SELECTOR_SZ];
    char       value[32];

    config__init_selectors(config);

    while(fgets(line, 80, config->File) != NULL)
    {
        if((line[0] == '#') || (line[0] <= space_or_control_ch))
        {
            continue;
        }
        // Splits a string around the " = ".
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
