#include "buffer.h"

bool buffer__init(Buff_t* Buffer)
{
    Buffer->path = malloc(PATH_MAX);
    if(Buffer->path == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for the directory.\n");
        return false;
    }

    // Pointer to pointers, so the address size is needed.
    Buffer->text = malloc(ADDR_SZ);
    if(Buffer->text == NULL)
    {
        fprintf(stderr, "Can't alloc a memory the array with lines.\n");
        return false;
    }

    Buffer->lines_length_idx = malloc(((sizeof(idx_t) / ADDR_SZ) * ADDR_SZ)
                                      + ADDR_SZ);

    if(Buffer->lines_length_idx == NULL)
    {
        fprintf(stderr, "Can't alloc a memory the array with lines length.\n");
        return false;
    }
    Buffer->chars_idx       = 0;
    Buffer->lines_idx       = 0;
    Buffer->cursor_rev_x    = 0;
    Buffer->cursor_rev_y    = 0;
    CURRENT_LINE_LENGTH_IDX = 0;
    Buffer->chars_sequence  = false;

    CURRENT_LINE = malloc(ADDR_SZ);
    if(CURRENT_LINE == NULL)
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
    if(Buffer->lines_length_idx != NULL)
    {
        free(Buffer->lines_length_idx);
    }
    if(Buffer->text != NULL)
    {
        free(Buffer->text);
    }
    if(Buffer->path != NULL)
    {
        free(Buffer->path);
    }
}
