#include "memory.h"

bool memory__extend_line(Buff_t* Buffer, const idx_t line_idx)
{
    idx_t memblock = BUFFER__MEMBLOCK;
    idx_t line_length = Buffer->Lines[line_idx].length;

    if(line_length == BUFFER__INITIAL_MEMBLOCK)
    {
        // There are 4/8 chars, extend to BUFFER__MEMBLOCK.
        Buffer->Lines[line_idx].text = realloc(Buffer->Lines[line_idx].text,
                                               memblock);

#ifdef DEBUG_MEMORY
        printf("Extend_line %u with mem of %u B.\n",
               line_idx + IDX, memblock);
#endif

    }
    else if((line_length > BUFFER__INITIAL_MEMBLOCK)
            && (line_length % BUFFER__MEMBLOCK == 0))
    {
        // There are more chars so append the new memblock.
        memblock = ((line_length / BUFFER__MEMBLOCK) * BUFFER__MEMBLOCK)
                   + BUFFER__MEMBLOCK;

        Buffer->Lines[line_idx].text = realloc(Buffer->Lines[line_idx].text,
                                               memblock);

#ifdef DEBUG_MEMORY
        printf("Extend_line %u with mem of %u B.\n",
               line_idx + IDX, memblock);
#endif

    }

    if(Buffer->Lines[line_idx].text == NULL)
    {
        fprintf(stderr, "Can't extend a memory block for the line.\n");
        return false;
    }
    return true;
}

bool memory__shrink_current_line(Buff_t* Buffer)
{
    idx_t memblock = BUFFER__INITIAL_MEMBLOCK;

    /* These cases are executed only when the backspace is pressed. Works in the
    same way as "extend_current_line". */
    if((BUFFER__CURRENT_LINE_LEN >= BUFFER__INITIAL_MEMBLOCK)
       && (BUFFER__CURRENT_LINE_LEN < BUFFER__MEMBLOCK))
    {
        // Shrink to size of the BUFFER__MEMBLOCK.
        memblock = BUFFER__MEMBLOCK;
    }
    else if(BUFFER__CURRENT_LINE_LEN >= BUFFER__MEMBLOCK)
    {
        // Remove the newest memblock because isn't needed now.
        memblock = ((BUFFER__CURRENT_LINE_LEN / BUFFER__MEMBLOCK)
                   * BUFFER__MEMBLOCK) + BUFFER__MEMBLOCK;
    }
    BUFFER__CURRENT_LINE = realloc(BUFFER__CURRENT_LINE, memblock);
    if(BUFFER__CURRENT_LINE == NULL)
    {
        fprintf(stderr, "Can't shrink the memblock with the current line.\n");
        return false;
    }

#ifdef DEBUG_MEMORY
    printf("Shrink_current_line %u with mem of %u B.\n",
           BUFFER__CURRENT_LINE_IDX + IDX, memblock);
#endif

    return true;
}

bool memory__shrink_prev_line(Buff_t* Buffer)
{
    idx_t memblock = BUFFER__INITIAL_MEMBLOCK;

    if((BUFFER__PREVIOUS_LINE_LEN >= BUFFER__INITIAL_MEMBLOCK)
       && (BUFFER__PREVIOUS_LINE_LEN < BUFFER__MEMBLOCK))
    {
        memblock = BUFFER__MEMBLOCK;
    }
    else if((BUFFER__PREVIOUS_LINE_LEN >= BUFFER__MEMBLOCK))
    {
        // Set the size of some MEMBLKs.
        memblock = ((BUFFER__PREVIOUS_LINE_LEN / BUFFER__MEMBLOCK)
                   * BUFFER__MEMBLOCK) + BUFFER__MEMBLOCK;
    }
    BUFFER__PREVIOUS_LINE = realloc(BUFFER__PREVIOUS_LINE, memblock);

#ifdef DEBUG_MEMORY
    printf("Shrink_PREV_line %u with mem of %u B\n",
           BUFFER__PREVIOUS_LINE_IDX + IDX, memblock);
#endif

    if(BUFFER__PREVIOUS_LINE == NULL)
    {
        fprintf(stderr, "Can't shrink the memblock with the PREVious line.\n");
        return false;
    }
    return true;
}

bool memory__extend_lines_array(Buff_t* Buffer)
{
    // Enhance the array that contains pointers to lines.
    Buffer->Lines = realloc(Buffer->Lines, (Buffer->lines_amount_idx + IDX)
                            * LINE__TYPE_SZ);

    if(Buffer->Lines == NULL)
    {
        fprintf(stderr, "Can't extend the array with lines.\n");
        return false;
    }

    // The new line is allocated with only 4 or 8 bytes bytes.
    if((BUFFER__LAST_LINE = malloc(BUFFER__INITIAL_MEMBLOCK)) == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for the new line.\n");
        return false;
    }

    // Naturally the new line doesn't contains any chars - only terminator.
    BUFFER__LAST_LINE_LEN = 0;

    return true;
}

bool memory__shrink_lines_array(Buff_t* Buffer)
{
    Buffer->Lines = realloc(Buffer->Lines, (Buffer->lines_amount_idx + IDX)
                            * LINE__TYPE_SZ);

    if(Buffer->Lines == NULL)
    {
        fprintf(stderr, "Can't shrink the array with lines.\n");
        return false;
    }
    return true;
}

bool memory__copy_lines_forward(Buff_t* Buffer)
{
    for(idx_t line_idx = Buffer->lines_amount_idx; line_idx > BUFFER__CURRENT_LINE_IDX;
        line_idx--)
    {
        idx_t memblock = ((Buffer->Lines[line_idx - PREV].length
                         / BUFFER__MEMBLOCK) * BUFFER__MEMBLOCK)
                         + BUFFER__MEMBLOCK;

        Buffer->Lines[line_idx].text = realloc(Buffer->Lines[line_idx].text,
                                               memblock);

#ifdef DEBUG_MEMORY
        printf("Copied line %u forward to %u with new mem of %u B.\n",
               line_idx + IDX - PREV, line_idx + IDX, memblock);
#endif

        if(Buffer->Lines[line_idx].text == NULL)
        {
            fprintf(stderr, "Can't resize the line forward.\n");
            return false;
        }
        strcpy(Buffer->Lines[line_idx].text,
               Buffer->Lines[line_idx - PREV].text);

        Buffer->Lines[line_idx].length = Buffer->Lines[line_idx - PREV].length;
    }
    return true;
}

bool memory__copy_lines_backward(Buff_t* Buffer)
{
    for(idx_t line_idx = BUFFER__CURRENT_LINE_IDX; line_idx < Buffer->lines_amount_idx;
        line_idx++)
    {
        idx_t memblock = ((Buffer->Lines[line_idx + NEXT].length
                         / BUFFER__MEMBLOCK) * BUFFER__MEMBLOCK)
                         + BUFFER__MEMBLOCK;

        Buffer->Lines[line_idx].text = realloc(Buffer->Lines[line_idx].text,
                                               memblock);

#ifdef DEBUG_MEMORY
        printf("Copied line %u backward to %u with new mem of %u B.\n",
               line_idx + IDX + NEXT, line_idx + IDX, memblock);
#endif

        if(Buffer->Lines[line_idx].text == NULL)
        {
            fprintf(stderr, "Can't copy a line backward.\n");
            return false;
        }
        strcpy(Buffer->Lines[line_idx].text,
               Buffer->Lines[line_idx + NEXT].text);

        Buffer->Lines[line_idx].length = Buffer->Lines[line_idx + NEXT].length;
    }
    return true;
}
