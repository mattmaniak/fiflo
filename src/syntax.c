#include "syntax.h"

bool syntax__load(Syntax_t* Syntax, const int extension)
{
    char  fname[64];
    char  keyword[16];
    char  color[16];
    idx_t color_idx = 0;
    FILE* File;

    if(extension == C)
    {
        strcpy(fname, "/usr/share/fiflo/c.fiflorc");
    }
    if((File = fopen(fname, "r")) != NULL)
    {
        while(fscanf(File, "%s = %s", keyword, color) != EOF)
        {
            strncpy(Syntax->Keywords[color_idx].keyword, keyword, 16);
            if(!(strcmp(color, "magenta")))
            {
                Syntax->Keywords[color_idx].color = MAGENTA;
            }
            else
            {
                Syntax->Keywords[color_idx].color = 0;
            }
            if(color_idx++ >= SYNTAX__MAX_KEYWORDS)
            {
                break;
            }
        }
        Syntax->kwrds_idx = --color_idx;
        if(fclose(File) == EOF)
        {
            fprintf(stderr, "Failed to close the syntax file.\n");
            return false;
        }
    }
    return true;
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
        }
    }
    return char_idx;
}
