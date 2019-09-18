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
        for(idx_t shift_idx = kwrd_idx + NEXT; shift_idx < Syntax->kwrds_idx;
            shift_idx++)
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
                         const Ui_t* const Ui, const idx_t end_ch_idx,
                         idx_t ch_idx)
{
    const char* const str_to_print_addr = &Line->txt[ch_idx];
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
            end_paint_idx = syntax__check_word_to_paint(Syntax, Line, ch_idx,
                                                        kwrd_idx);
            // Breaks a word if the end of a terminal is achieved.
            if(end_paint_idx > end_ch_idx)
            {
                end_paint_idx = end_ch_idx;
            }
            for(; ch_idx < end_paint_idx; ch_idx++)
            {
                if(ch_idx == (const idx_t) (Config->Punch_card_width.value - IDX + Ui->pcard_delta_x))
                {
                    ui__colorize(Config->Color_ui.value + 10);
                    ui__colorize(Syntax->Keywords[kwrd_idx].color);
                }
                // ui__colorize(Syntax->Keywords[kwrd_idx].color);
                // putchar('!');
                putchar(Line->txt[ch_idx]);
                ui__colorize(0);
            }
            if(ch_idx < end_ch_idx) // Not a last char, so don't hide a next.
            {
                ch_idx--; // An other char will be printed. Make it visible.
            }
            break;
        }
    }
    return ch_idx;
}

idx_t syntax__check_word_to_paint(const Syntax_t* const Syntax,
                                  const Line_t* const Line, const idx_t ch_idx,
                                  const idx_t kwrd_idx)
{
    const idx_t end_paint_idx = ch_idx
    + (const idx_t) strlen(Syntax->Keywords[kwrd_idx].keyword);

    const bool allowed_sufix = (Line->txt[end_paint_idx] == ' ')
                               || (Line->txt[end_paint_idx] == '\n')
                               || (Line->txt[end_paint_idx] == '\t')
                               || (Line->txt[end_paint_idx] == '(')
                               || (Line->txt[end_paint_idx] == '*')
                               || (Line->txt[end_paint_idx] == '\0');

    // A word at the beginning of the line.
    if((ch_idx == 0)
       && (allowed_sufix || (Line->txt[end_paint_idx] == ':')))
    {
        ui__colorize(Syntax->Keywords[kwrd_idx].color);
    }
    // Another word somewhere in a text.
    else if(((Line->txt[ch_idx - PREV] == ' ')
             || (Line->txt[ch_idx - PREV] == '\t')
             || (Line->txt[ch_idx - PREV] == '('))
            && (allowed_sufix || (Line->txt[end_paint_idx] == ')')))
    {
        ui__colorize(Syntax->Keywords[kwrd_idx].color);
    }
    return end_paint_idx;
}
