#include "syntax.h"

bool syntax__load(Syntax_t* const Syntax, const int extension)
{
    char  fname[64];
    char  keyword[SYNTAX__MAX_KWRD_LEN];
    char  color[SYNTAX__MAX_KWRD_LEN];
    FILE* file;

    Syntax->kwrds_idx = 0;

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

    if((file = fopen(fname, "r")) != NULL)
    {
        while(fscanf(file, "%s = %s", keyword, color) != EOF)
        {
            strncpy(Syntax->Keywords[Syntax->kwrds_idx].keyword, keyword,
                    SYNTAX__MAX_KWRD_LEN);

            Syntax->Keywords[Syntax->kwrds_idx].color =
            config__parse_value(color);

            if(Syntax->kwrds_idx++ >= SYNTAX__MAX_KWRDS_IN_FILE)
            {
                break;
            }
        }
        Syntax->kwrds_idx--;
        syntax__sort(Syntax);

        if(fclose(file) == EOF)
        {
            fprintf(stderr, "Failed to close a syntax file.\n");
            return false;
        }
    }
    return true;
}

void syntax__sort(Syntax_t* const Syntax)
{
    Syntax__Kwrd_t Tmp_kwrd;

    // Iterate through all keywords.
    for(idx_t kwrd_idx = 0; kwrd_idx < Syntax->kwrds_idx; kwrd_idx++)
    {
        // Set a value of a further keyword to compare.
        for(idx_t shift_idx = kwrd_idx + SIZE__NEXT;
            shift_idx < Syntax->kwrds_idx; shift_idx++)
        {
            // Swap if a next keyword is longer.
            if(strlen(Syntax->Keywords[kwrd_idx].keyword)
               < strlen(Syntax->Keywords[shift_idx].keyword))
            {
                Tmp_kwrd = Syntax->Keywords[kwrd_idx];

                Syntax->Keywords[kwrd_idx]  = Syntax->Keywords[shift_idx];
                Syntax->Keywords[shift_idx] = Tmp_kwrd;
            }
        }
    }
}

idx_t syntax__paint_word(const Syntax_t* const Syntax,
                         const Conf_t* const Config, Line_t* Line,
                         const idx_t end_ch_idx, idx_t ch_idx)
{
    const char* const str_to_print_addr = &Line->txt[ch_idx];
    const idx_t       pcard_w           = (const idx_t) Config->Pcard_w.value;
    bool              ignored_kwrd      = false;
    idx_t             end_paint_idx;

    if(Syntax->kwrds_idx <= 0)
    {
        return ch_idx;
    }
    for(idx_t kwrd_idx = 0; kwrd_idx <= Syntax->kwrds_idx; kwrd_idx++)
    {
        if(strstr(str_to_print_addr, Syntax->Keywords[kwrd_idx].keyword)
           == str_to_print_addr)
        {
            ui__colorize(Syntax->Keywords[kwrd_idx].color);

            end_paint_idx = ch_idx + (const idx_t)
                            strlen(Syntax->Keywords[kwrd_idx].keyword);

            if(end_paint_idx == 0)
            {
                end_paint_idx = ch_idx;
                ignored_kwrd  = true;
            }

            // Breaks a word if the end of a terminal is achieved.
            if(end_paint_idx > end_ch_idx)
            {
                end_paint_idx = end_ch_idx;
            }
            for(; ch_idx < end_paint_idx; ch_idx++)
            {
                if(ch_idx == (pcard_w - SIZE__IDX))
                {
                    ui__colorize(Config->Color_ui.value
                                 + ANSI__BG_COLOR_OFFSET);
                    ui__colorize(Syntax->Keywords[kwrd_idx].color);
                }
                putchar(Line->txt[ch_idx]);
                if(ch_idx == (pcard_w - SIZE__IDX))
                {
                    // Reset the background color after punch card.
                    ui__colorize(0);
                    ui__colorize(Syntax->Keywords[kwrd_idx].color);
                }
            }
            // Not a last char, so don't hide a next.
            if(!ignored_kwrd && (ch_idx < end_ch_idx))
            {
                ch_idx--; // An other char will be printed. Make it visible.
            }
            break;
        }
    }
    return ch_idx;
}
