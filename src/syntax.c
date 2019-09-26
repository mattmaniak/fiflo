#include "syntax.h"

bool syntax__load(Syntax_t* const Syntax, const int extension)
{
    char  fname[64];
    char  keyword[SYNTAX__MAX_KWRD_LEN];
    char  color[SYNTAX__MAX_KWRD_LEN];
    FILE* File;

    Syntax->kwrds_amount = 0;

    switch(extension)
    {
    case EXTENSION__C:
        strcpy(fname, "/usr/share/fiflo/c.fiflorc");
        break;

    case EXTENSION__PYTHON:
        strcpy(fname, "/usr/share/fiflo/python.fiflorc");
        break;

    case EXTENSION__HTML:
        strcpy(fname, "/usr/share/fiflo/html.fiflorc");
    }

    File = fopen(fname, "r");
    if(File != NULL)
    {
        while(fscanf(File, "%s = %s", keyword, color) != EOF)
        {
            strncpy(Syntax->Keywords[Syntax->kwrds_amount].keyword, keyword,
                    SYNTAX__MAX_KWRD_LEN);

            Syntax->Keywords[Syntax->kwrds_amount].color =
            config__parse_value(color);

            if(Syntax->kwrds_amount++ >= SYNTAX__MAX_KWRDS_IN_FILE)
            {
                break;
            }
        }
        Syntax->kwrds_amount--;
        syntax__sort(Syntax);

        if(fclose(File) == EOF)
        {
            fprintf(stderr, "Failed to close a syntax File.\n");
            return false;
        }
    }
    return true;
}

void syntax__sort(Syntax_t* const Syntax)
{
    Syntax__Kwrd_t Tmp_kwrd;

    // Iterate through all keywords.
    for(size_t kwrd_i = 0; kwrd_i < Syntax->kwrds_amount; kwrd_i++)
    {
        // Set a value of a further keyword to compare.
        for(size_t shift_i = kwrd_i + SIZE__NEXT;
            shift_i < Syntax->kwrds_amount; shift_i++)
        {
            // Swap if a next keyword is longer.
            if(strlen(Syntax->Keywords[kwrd_i].keyword)
               < strlen(Syntax->Keywords[shift_i].keyword))
            {
                Tmp_kwrd = Syntax->Keywords[kwrd_i];

                Syntax->Keywords[kwrd_i]  = Syntax->Keywords[shift_i];
                Syntax->Keywords[shift_i] = Tmp_kwrd;
            }
        }
    }
}

size_t syntax__paint_word(const Syntax_t* const Syntax,
                          const Conf_t* const Config, Line_t* Line,
                          const size_t end_ch_i, size_t ch_i)
{
    const char* const str_to_print_addr = &Line->txt[ch_i];
    const size_t      pcard_w           = (size_t) Config->Pcard_w.value;
    bool              kwrd_ignored      = false;
    size_t            end_paint_i;

    if(Syntax->kwrds_amount <= 0)
    {
        return ch_i;
    }
    for(size_t kwrd_i = 0; kwrd_i <= Syntax->kwrds_amount; kwrd_i++)
    {
        if(strstr(str_to_print_addr, Syntax->Keywords[kwrd_i].keyword)
           == str_to_print_addr)
        {
            ui__colorize(Syntax->Keywords[kwrd_i].color);

            end_paint_i = (size_t) strlen(Syntax->Keywords[kwrd_i].keyword)
                          + ch_i;

            if(end_paint_i == 0)
            {
                end_paint_i  = ch_i;
                kwrd_ignored = true;
            }

            // Breaks a word if the end of a terminal is achieved.
            if(end_paint_i > end_ch_i)
            {
                end_paint_i = end_ch_i;
            }
            for(; ch_i < end_paint_i; ch_i++)
            {
                if(ch_i == (pcard_w - SIZE__I))
                {
                    ui__colorize(Config->Color_ui.value
                                 + ANSI__BG_COLOR_OFFSET);
                    ui__colorize(Syntax->Keywords[kwrd_i].color);
                }
                putchar(Line->txt[ch_i]);
                if(ch_i == (pcard_w - SIZE__I))
                {
                    // Reset the background color after punch card.
                    ui__colorize(0);
                    ui__colorize(Syntax->Keywords[kwrd_i].color);
                }
            }
            // Not a last char, so don't hide a next.
            if(!kwrd_ignored && (ch_i < end_ch_i))
            {
                ch_i--; // An other char will be printed. Make it visible.
            }
            break;
        }
    }
    return ch_i;
}
