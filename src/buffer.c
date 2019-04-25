#include "buffer.h"

bool buffer__init(Buff_t* Buffer)
{
    if((Buffer->pathname = malloc(PATH_MAX)) == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for the directory.\n");
        return false;
    }

    // Pointer to pointers, so the address size is needed.
    if((Buffer->text = malloc(ADDR_SZ)) == NULL)
    {
        fprintf(stderr, "Can't alloc a memory the array with lines.\n");
        return false;
    }

    Buffer->lines_length = malloc(((sizeof(idx_t) / ADDR_SZ) * ADDR_SZ)
                                  + ADDR_SZ);

    if(Buffer->lines_length == NULL)
    {
        fprintf(stderr, "Can't alloc a memory the array with lines length.\n");
        return false;
    }
    Buffer->chars_idx    = 0;
    Buffer->lines_idx    = 0;
    Buffer->cursor_rev_x = 0;
    Buffer->cursor_rev_y = 0;
    CURRENT_LINE_LENGTH  = 0;

    Buffer->escape_sequence_on_input = false;

    if((CURRENT_LINE = malloc(ADDR_SZ)) == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for the first line.\n");
        return false;
    }
    return true;
}

void buffer__free(Buff_t* Buffer)
{
    for(idx_t line_idx = 0; line_idx <= Buffer->lines_idx; line_idx++)
    {
        if(Buffer->text[line_idx] != NULL)
        {
            free(Buffer->text[line_idx]);
        }
    }
    if(Buffer->lines_length != NULL)
    {
        free(Buffer->lines_length);
    }
    if(Buffer->text != NULL)
    {
        free(Buffer->text);
    }
    if(Buffer->pathname != NULL)
    {
        free(Buffer->pathname);
    }
}
