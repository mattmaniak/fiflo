#include "config.h"

bool config__load(Conf_t* const Config)
{
    const char conf_fname[] = "/etc/fiflorc";

    if(access(conf_fname, F_OK) == -1) // There is no config file.
    {
        config__set_default(Config);
        return true;
    }
    Config->File = fopen(conf_fname, "r");
    if(Config->File != NULL)
    {
        config__load_custom(Config);

        if(fclose(Config->File) == EOF)
        {
            fprintf(stderr, "Unable to close a configuration file.\n");
            return false;
        }
    }
    return true;
}

void config__init_selectors(Conf_t* const Config)
{
    strcpy(Config->Color_txt.selector,        "color_text");
    strcpy(Config->Color_ui.selector,         "color_ui");
    strcpy(Config->Color_whitespace.selector, "color_whitespace");
    strcpy(Config->Pcard_w.selector,          "punch_card_width");
    strcpy(Config->Tab_sz.selector,           "tab_size");
}

bool config__parse_selector(Conf_t* const Config, const char* const selector,
                            const int value)
{
    // Adds a value to a found selector in a configuration structure.
    if(value >= RED)
    {
        if(!strcmp(Config->Color_ui.selector, selector))
        {
            Config->Color_ui.value = value;
        }
        else if(!strcmp(Config->Color_txt.selector, selector))
        {
            Config->Color_txt.value = value;
        }
        else if(!strcmp(Config->Color_whitespace.selector, selector))
        {
            Config->Color_whitespace.value = value;
        }
        else if(!strcmp(Config->Pcard_w.selector, selector))
        {
            Config->Pcard_w.value = value;
        }
    }
    else if(!strcmp(Config->Tab_sz.selector, selector))
    {
        if((value >= CONFIG__MIN_TAB_SZ) && (value <= CONFIG__MAX_TAB_SZ))
        {
            Config->Tab_sz.value = value;
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

void config__set_default(Conf_t* const Config)
{
    Config->Color_txt.value         = WHITE;
    Config->Color_ui.value          = WHITE;
    Config->Color_whitespace.value  = BRIGHT_BLACK;
    Config->Pcard_w.value           = CONFIG__PUNCH_CARD_W;
    Config->Tab_sz.value            = CONFIG__MAX_TAB_SZ;
}

void config__load_custom(Conf_t* const Config)
{
    const char space_or_control_ch = 32;
    int        parsed_value        = 0;
    char       line[80];
    char       selector[CONFIG__SELECTOR_SZ];
    char       value[32];

    config__init_selectors(Config);

    while(fgets(line, 80, Config->File) != NULL)
    {
        if((line[0] == '#') || (line[0] <= space_or_control_ch))
        {
            continue;
        }
        // Splits a string around the " = ".
        strncpy(selector, strtok(line, " = "), CONFIG__SELECTOR_SZ);
        strncpy(value,    strtok(NULL, " = "), 32);

        parsed_value = config__parse_value(value);

        if(!config__parse_selector(Config, selector, parsed_value))
        {
            config__set_default(Config);
            break;
        }
    }
    if(parsed_value == 0) // If a whole file is commented out.
    {
        config__set_default(Config);
    }
}
