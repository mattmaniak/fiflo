#include "config.h"

bool config__load(Conf_t* const Config)
{
    char conf_fname[] = "/etc/fiflorc";

    if(access(conf_fname, F_OK) == ERROR) // There is no config file.
    {
        config__set_default(Config);
        return true;
    }
    if((Config->File = fopen(conf_fname, "r")) != NULL)
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
    strcpy(Config->Color_actual_file.selector, "color_actual_file");
    strcpy(Config->Color_line_number.selector, "color_line_number");
    strcpy(Config->Color_text.selector,        "color_text");
    strcpy(Config->Color_ui.selector,          "color_ui");
    strcpy(Config->Color_warning.selector,     "color_warning");
    strcpy(Config->Color_whitespace.selector,  "color_whitespace");
    strcpy(Config->Punch_card_width.selector,  "punch_card_width");
    strcpy(Config->Tab_sz.selector,            "tab_size");
    strcpy(Config->Color_line_number_actual.selector,
           "color_line_number_actual");
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
        else if(!strcmp(Config->Color_actual_file.selector, selector))
        {
            Config->Color_actual_file.value = value;
        }
        else if(!strcmp(Config->Color_line_number_actual.selector, selector))
        {
            Config->Color_line_number_actual.value = value;
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
        else if(!strcmp(Config->Punch_card_width.selector, selector))
        {
            Config->Punch_card_width.value = value;
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
    Config->Color_actual_file.value        = BRIGHT_BLACK;
    Config->Color_line_number.value        = WHITE;
    Config->Color_line_number_actual.value = BRIGHT_BLACK;
    Config->Color_text.value               = WHITE;
    Config->Color_ui.value                 = GREEN;
    Config->Color_warning.value            = RED;
    Config->Color_whitespace.value         = BRIGHT_BLACK;
    Config->Punch_card_width.value         = CONFIG__PUNCH_CARD_W;
    Config->Tab_sz.value                   = CONFIG__MAX_TAB_SZ;
}

void config__load_custom(Conf_t* const Config)
{
    const char space_or_control_char = 32;
    int        parsed_value = 0;
    char       line[80];
    char       selector[CONFIG__SELECTOR_SZ];
    char       value[32];

    config__init_selectors(Config);

    while(fgets(line, 80, Config->File) != NULL)
    {
        if((line[0] == '#') || (line[0] <= space_or_control_char))
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
