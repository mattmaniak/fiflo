#include "syntax.h"

bool syntax__load(Syntax_t* Syntax, const int extension)
{
    char  fname[64];
    char  keyword[SYNTAX__MAX_KWRD_LEN];
    char  color[SYNTAX__MAX_KWRD_LEN];
    FILE* File;

    Syntax->kwrds_idx = 0;

    switch(extension)
    {
    case C:
        strcpy(fname, "/usr/share/fiflo/c.fiflorc");
        break;

    case PYTHON:
        strcpy(fname, "/usr/share/fiflo/python.fiflorc");
    }

    if((File = fopen(fname, "r")) != NULL)
    {
        while(fscanf(File, "%s = %s", keyword, color) != EOF)
        {
            strncpy(Syntax->Keywords[Syntax->kwrds_idx].keyword, keyword,
                    SYNTAX__MAX_KWRD_LEN);

            if(!(strcmp(color, "magenta")))
            {
                Syntax->Keywords[Syntax->kwrds_idx].color = MAGENTA;
            }
            else
            {
                Syntax->Keywords[Syntax->kwrds_idx].color = 0;
            }
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

void syntax__sort(Syntax_t* Syntax)
{
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
                const Syntax__Kwrd_t Temp_keyword = Syntax->Keywords[kwrd_idx];

                Syntax->Keywords[kwrd_idx]  = Syntax->Keywords[shift_idx];
                Syntax->Keywords[shift_idx] = Temp_keyword;
            }
        }
        puts(Syntax->Keywords[kwrd_idx].keyword);
    }
}

idx_t syntax__paint_word(const Syntax_t* const Syntax, Line_t* Line,
                         idx_t char_idx)
{
    const char* const str_to_print_addr = &Line->text[char_idx];

    if(Syntax->kwrds_idx <= 0)
    {
        return char_idx;
    }
    for(idx_t kwrd_idx = 0; kwrd_idx <= Syntax->kwrds_idx; kwrd_idx++)
    {
        if(strstr(str_to_print_addr, Syntax->Keywords[kwrd_idx].keyword)
           == str_to_print_addr)
        {
            const idx_t end_color_offset = char_idx
            + (const idx_t) strlen(Syntax->Keywords[kwrd_idx].keyword);

            ui__set_color(&Syntax->Keywords[kwrd_idx].color);
            for(; char_idx < end_color_offset; char_idx++)
            {
                putchar(Line->text[char_idx]);
            }
            char_idx--;

            break;
        }
    }
    return char_idx;
}
