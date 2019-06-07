#include "config.h"

bool config__load(Conf_t* Config)
{
    char config_fname[] = "/etc/fiflorc";

    if(access(config_fname, F_OK) == ERROR) // There is no config file.
    {
        config__set_default(Config);
        return true;
    }

    Config->File = fopen(config_fname, "r");
    if(Config->File != NULL)
    {
        config__load_custom(Config);

        if(fclose(Config->File) == EOF)
        {
            fprintf(stderr, "Can't close the config file.\n");
            return false;
        }
    }
    return true;
}

void config__init_selectors(Conf_t* Config)
{
    strcpy(Config->Color_line_number_current.selector,
           "color-line-number-current");

    strcpy(Config->Color_line_number.selector, "color-line-number");
    strcpy(Config->Color_ui.selector,          "color-ui");
    strcpy(Config->Color_text.selector,        "color-text");
    strcpy(Config->Color_warning.selector,     "color-warning");
    strcpy(Config->Color_whitespace.selector,  "color-whitespace");
    strcpy(Config->Tab_sz.selector,            "tab-size");
}

bool config__parse_selector(Conf_t* Config, const char* const selector,
                            const int value)
{
    // Adds the value to the found selector in the configuration.
    if(value >= CONFIG__RED)
    {
        if(!strcmp(Config->Color_ui.selector, selector))
        {
            Config->Color_ui.value = value;
        }
        else if(!strcmp(Config->Color_line_number_current.selector, selector))
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
        else if(!strcmp(Config->Color_whitespace.selector, selector))
        {
            Config->Color_whitespace.value = value;
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
    if(!strcmp(read_value, "red\n"))
    {
        return CONFIG__RED;
    }
    else if(!strcmp(read_value, "green\n"))
    {
        return CONFIG__GREEN;
    }
    else if(!strcmp(read_value, "yellow\n"))
    {
        return CONFIG__YELLOW;
    }
    else if(!strcmp(read_value, "blue\n"))
    {
        return CONFIG__BLUE;
    }
    else if(!strcmp(read_value, "magenta\n"))
    {
        return CONFIG__MAGENTA;
    }
    else if(!strcmp(read_value, "cyan\n"))
    {
        return CONFIG__CYAN;
    }
    else if(!strcmp(read_value, "white\n"))
    {
        return CONFIG__WHITE;
    }
    else if(!strcmp(read_value, "bright-black\n"))
    {
        return CONFIG__BRIGHT_BLACK;
    }
    else if(!strcmp(read_value, "bright-red\n"))
    {
        return CONFIG__BRIGHT_RED;
    }
    else if(!strcmp(read_value, "bright-green\n"))
    {
        return CONFIG__BRIGHT_GREEN;
    }
    else if(!strcmp(read_value, "bright-yellow\n"))
    {
        return CONFIG__BRIGHT_YELLOW;
    }
    else if(!strcmp(read_value, "bright-blue\n"))
    {
        return CONFIG__BRIGHT_BLUE;
    }
    else if(!strcmp(read_value, "bright-magenta\n"))
    {
        return CONFIG__BRIGHT_MAGENTA;
    }
    else if(!strcmp(read_value, "bright-cyan\n"))
    {
        return CONFIG__BRIGHT_CYAN;
    }
    else if(!strcmp(read_value, "bright-white\n"))
    {
        return CONFIG__BRIGHT_WHITE;
    }
    return atoi(read_value); // Seems like linefeed removal too.
}

void config__set_default(Conf_t* Config)
{
    Config->Color_ui.value                  = CONFIG__WHITE;
    Config->Color_line_number.value         = CONFIG__WHITE;
    Config->Color_line_number_current.value = CONFIG__BRIGHT_BLACK;
    Config->Color_text.value                = CONFIG__WHITE;
    Config->Color_warning.value             = CONFIG__RED;
    Config->Color_whitespace.value          = CONFIG__BRIGHT_BLACK;
    Config->Tab_sz.value                    = CONFIG__MAX_TAB_SZ;
}

void config__load_custom(Conf_t* Config)
{
    int  parsed_value = 0;
    char line[80];
    char selector[48];
    char value[32];

    config__init_selectors(Config);

    while(fgets(line, 80, Config->File) != NULL)
    {
        if((line[0] == '#') || (line[0] <= 32)) // Hash, space or unseen char.
        {
            continue;
        }
        // Splits the string around the " = ".
        strncpy(selector, strtok(line, " = "), 48);
        strncpy(value,    strtok(NULL, " = "), 32);

        parsed_value = config__parse_value(value);

        if(!config__parse_selector(Config, selector, parsed_value))
        {
            config__set_default(Config);
            break;
        }
    }
    if(parsed_value == 0) // If the whole file is commented out.
    {
        config__set_default(Config);
    }
}
