#include "v_file.h"

bool buffer__init(V_file_t* const V_file)
{
    V_file->pathname = malloc(PATH_MAX);
    if(V_file->pathname == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for a directory.\n");
        return false;
    }

    // Pointer to pointers, so an address size is required.
    V_file->Lines = malloc(sizeof(Line_t));
    if(V_file->Lines == NULL)
    {
        fprintf(stderr, "Can't alloc a memory a array with lines.\n");
        return false;
    }
    V_file->ch_amount        = 0;
    V_file->ln_amount        = 0;
    V_file->cursor_rev_x     = 0;
    V_file->cursor_rev_y     = 0;
    V_FILE__ACTUAL_LN.len    = 0;
    V_file->esc_seq_on_input = false;
    V_file->extension        = EXTENSION__NONE;

    V_FILE__ACTUAL_LN.txt = malloc(V_FILE__BASIC_MEMBLK);
    if(V_FILE__ACTUAL_LN.txt == NULL)
    {
        fprintf(stderr, "Can't allocate a memory for a first line.\n");
        return false;
    }
    return true;
}

void buffer__free(V_file_t* const V_file)
{
    for(size_t ln_i = 0; ln_i <= V_file->ln_amount; ln_i++)
    {
        if(V_file->Lines[ln_i].txt != NULL)
        {
            free(V_file->Lines[ln_i].txt);
        }
    }
    if(V_file->Lines != NULL)
    {
        free(V_file->Lines);
    }
    if(V_file->pathname != NULL)
    {
        free(V_file->pathname);
    }
}
