#include "highlighter.h"

// bool highlighter__load()
// {
//     const char fname[] = "/usr/share/nano/";
//     return true;
// }

idx_t highlighter__paint_word(Line_t* Line, idx_t char_idx)
{
    const char        keyword[]         = "void";
    const char* const str_to_print_addr = &Line->text[char_idx];
    const char* const found_word_addr   = strstr(str_to_print_addr, keyword);

    if(found_word_addr == str_to_print_addr)
    {
        const idx_t end_color_offset = (const idx_t) strlen(keyword)
                                       + char_idx;

        printf("\033[35m"); // TODO: TEMPONARY.
        for(; char_idx < end_color_offset; char_idx++)
        {
            putchar(Line->text[char_idx]);
        }
        char_idx--;
    }
    return char_idx;
}
