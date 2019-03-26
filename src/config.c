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
    strcpy(Config->Color_whitespace.selector,  "color-whitespace");
}

void config__parse_selector(Conf_t* Config, char* selector, int value)
{
    // Adds the value to the found selector in the configuration.
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
    else if(!strcmp(Config->Tab_width.selector, selector))
    {
        Config->Tab_width.value = value;
    }
    else if(!strcmp(Config->Color_whitespace.selector, selector))
    {
        Config->Color_whitespace.value = value;
    }
}

int config__encode_color(Conf_t* Config, char* color_name)
{
    int value = 0;

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
    else if(!strcmp(color_name, "bright-black\n"))
    {
        value = BRIGHT_BLACK;
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
            || !strcmp(color_name, "8\n")) // Supported tab sizes.
    {
        value = atoi(color_name); // Seems like linefeed removal too.
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
    Config->Color_line_number.value         = WHITE;
    Config->Color_line_number_current.value = BRIGHT_BLACK;
    Config->Color_text.value                = WHITE;
    Config->Color_warning.value             = RED;
    Config->Color_whitespace.value          = BRIGHT_BLACK;
    Config->Tab_width.value                 = 8;
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
        if((line[0] == '#') || (line[0] <= 32))
        {
            continue;
        }
        // Splits around the " = ".
        strncpy(selector, strtok(line, " = "), 48);
        strncpy(value,    strtok(NULL, " = "), 32);

        parsed_value = config__encode_color(Config, value);
        config__parse_selector(Config, selector, parsed_value);
    }
    if(parsed_value == 0) // If the whole file is commented out.
    {
        config__set_default(Config);
    }
}
