#include "buffer.h"

bool buffer__init(Buff_t* Buffer)
{
    if((Buffer->pathname = malloc(PATH_MAX)) == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for a directory.\n");
        return false;
    }

    // Pointer to pointers, so a address size is needed.
    if((Buffer->Lines = malloc(sizeof(Line_t))) == NULL)
    {
        fprintf(stderr, "Can't alloc a memory a array with lines.\n");
        return false;
    }
    Buffer->chars_amount    = 0;
    Buffer->lines_amount    = 0;
    Buffer->cursor_rev_x    = 0;
    Buffer->cursor_rev_y    = 0;
    BUFFER__ACTUAL_LINE_LEN = 0;

    Buffer->escape_sequence_on_input = false;

    if((BUFFER__ACTUAL_LINE = malloc(BUFFER__BASIC_MEMBLK)) == NULL)
    {
        fprintf(stderr, "Can't allocate a memory for a first line.\n");
        return false;
    }
    return true;
}

void buffer__free(Buff_t* Buffer)
{
    for(idx_t ln_idx = 0; ln_idx <= Buffer->lines_amount; ln_idx++)
    {
        if(Buffer->Lines[ln_idx].text != NULL)
        {
            free(Buffer->Lines[ln_idx].text);
        }
    }
    if(Buffer->Lines != NULL)
    {
        free(Buffer->Lines);
    }
    if(Buffer->pathname != NULL)
    {
        free(Buffer->pathname);
    }
}
