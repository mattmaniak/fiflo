#include "buffer.h"

bool buffer__init(Buff_t* const Buffer)
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
    Buffer->ch_amount        = 0;
    Buffer->ln_amount        = 0;
    Buffer->cursor_rev_x     = 0;
    Buffer->cursor_rev_y     = 0;
    BUFFER__ACTUAL_LN.len    = 0;
    Buffer->esc_seq_on_input = false;

    if((BUFFER__ACTUAL_LN.txt = malloc(BUFFER__BASIC_MEMBLK)) == NULL)
    {
        fprintf(stderr, "Can't allocate a memory for a first line.\n");
        return false;
    }
    return true;
}

void buffer__free(Buff_t* const Buffer)
{
    for(size_t ln_i = 0; ln_i <= Buffer->ln_amount; ln_i++)
    {
        if(Buffer->Lines[ln_i].txt != NULL)
        {
            free(Buffer->Lines[ln_i].txt);
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
