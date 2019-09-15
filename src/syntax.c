#include "syntax.h"

bool syntax__load(Syntax_t* const Syntax, const int extension)
{
    char  fname[64];
    char  keyword[SYNTAX__MAX_KWRD_LEN];
    char  color[SYNTAX__MAX_KWRD_LEN];
    FILE* File;

    Syntax->kwrds_idx = 0;

    switch(extension)
    {
    case EXTENSION__C:
        strcpy(fname, "/usr/share/fiflo/c.fiflorc");
        break;

    case EXTENSION__PYTHON:
        strcpy(fname, "/usr/share/fiflo/python.fiflorc");
    }

    if((File = fopen(fname, "r")) != NULL)
    {
        while(fscanf(File, "%s = %s", keyword, color) != EOF)
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

        if(fclose(File) == EOF)
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

idx_t syntax__paint_word(const Syntax_t* const Syntax, Line_t* Line,
                         idx_t ch_idx)
{
    const char* const str_to_print_addr = &Line->text[ch_idx];
    idx_t             end_paint_offset;

    if(Syntax->kwrds_idx <= 0)
    {
        return ch_idx;
    }
    for(idx_t kwrd_idx = 0; kwrd_idx <= Syntax->kwrds_idx; kwrd_idx++)
    {
        if(strstr(str_to_print_addr, Syntax->Keywords[kwrd_idx].keyword)
           == str_to_print_addr)
        {
            end_paint_offset = syntax__check_word_to_paint(Syntax, Line,
                                                           ch_idx, kwrd_idx);

            for(; ch_idx < end_paint_offset; ch_idx++)
            {
                if(ch_idx == 79)
                {
                    printf("\033[7m");
                }
                putchar(Line->text[ch_idx]);
                if(ch_idx == 79)
                {
                    printf("\033[0m");
                    ui__colorize(&Syntax->Keywords[kwrd_idx].color);
                }
            }
            ch_idx--;

            break;
        }
    }
    return ch_idx;
}

idx_t syntax__check_word_to_paint(const Syntax_t* const Syntax,
                                  const Line_t* const Line, const idx_t ch_idx,
                                  const idx_t kwrd_idx)
{
    const idx_t end_paint_offset = ch_idx
    + (const idx_t) strlen(Syntax->Keywords[kwrd_idx].keyword);

    const bool allowed_sufix = (Line->text[end_paint_offset] == ' ')
                               || (Line->text[end_paint_offset] == '\n')
                               || (Line->text[end_paint_offset] == '(')
                               || (Line->text[end_paint_offset] == '*');

    // A word at the beginning of the line.
    if((ch_idx == 0) && (allowed_sufix
                         || (Line->text[end_paint_offset] == ':')))
    {
        ui__colorize(&Syntax->Keywords[kwrd_idx].color);
    }
    // Another word somewhere in a text.
    else if(((Line->text[ch_idx - PREV] == ' ')
             || (Line->text[ch_idx - PREV] == '\t')
             || (Line->text[ch_idx - PREV] == '('))
            && (allowed_sufix || (Line->text[end_paint_offset] == ')')))
    {
        ui__colorize(&Syntax->Keywords[kwrd_idx].color);
    }
    return end_paint_offset;
}
