#include "highlighter.h"

bool highlighter__paint_word(Line_t* Line, idx_t* char_idx)
{
    const char* str_to_print_addr = &Line->text[*char_idx];

    const char  keyword[]       = "void";
    const char* found_word_addr = strstr(str_to_print_addr, keyword);

    if(found_word_addr == str_to_print_addr)
    {
        // idx_t end_color_offset = *char_idx + (idx_t) strlen(keyword);

        // ui__set_color(&Config->Color_warning);
        // for(; *char_idx < end_color_offset; *char_idx++)
        // {
        //     putchar(Line->text[*char_idx]);
        // }
        // *char_idx--;

        return true;
    }
    else
    {
        return false;
    }
}
