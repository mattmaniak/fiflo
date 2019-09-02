#include "buffer.h"

bool buffer__init(Buff_t* Buffer)
{
    if((Buffer->pathname = malloc(PATH_MAX)) == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for the directory.\n");
        return false;
    }

    // Pointer to pointers, so the address size is needed.
    if((Buffer->Lines = malloc(LINE__TYPE_SZ)) == NULL)
    {
        fprintf(stderr, "Can't alloc a memory the array with lines.\n");
        return false;
    }
    Buffer->chars_amount_idx     = 0;
    Buffer->lines_amount_idx     = 0;
    Buffer->cursor_rev_x         = 0;
    Buffer->cursor_rev_y         = 0;
    BUFFER__CURRENT_LINE_LEN     = 0;

    Buffer->Lines[BUFFER__CURRENT_LINE_IDX].flexed_tab_end_offset = 0;

    Buffer->escape_sequence_on_input = false;

    if((BUFFER__CURRENT_LINE = malloc(BUFFER__INITIAL_MEMBLOCK)) == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for the first line.\n");
        return false;
    }
    return true;
}

void buffer__free(Buff_t* Buffer)
{
    for(idx_t line_idx = 0; line_idx <= Buffer->lines_amount_idx; line_idx++)
    {
        if(Buffer->Lines[line_idx].text != NULL)
        {
            free(Buffer->Lines[line_idx].text);
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
