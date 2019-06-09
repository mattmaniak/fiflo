#include "memory.h"

bool memory__extend_line(Buff_t* Buffer, const idx_t line_idx)
{
    idx_t memblock = BUFFER__MEMBLOCK;

    if(Buffer->lines_length[line_idx] == BUFFER__INITIAL_MEMBLOCK)
    {
        // There are 4/8 chars, extend to BUFFER__MEMBLOCK.
        Buffer->text[line_idx] = realloc(Buffer->text[line_idx], memblock);

#ifdef DEBUG_MEMORY
        printf("Extend_line %u with mem of %u B.\n",
               line_idx + IDX, memblock);
#endif

    }
    else if((Buffer->lines_length[line_idx] > BUFFER__INITIAL_MEMBLOCK)
            && (Buffer->lines_length[line_idx] % BUFFER__MEMBLOCK == 0))
    {
        // There are more chars so append the new memblock.
        memblock = ((Buffer->lines_length[line_idx] / BUFFER__MEMBLOCK)
                    * BUFFER__MEMBLOCK) + BUFFER__MEMBLOCK;

        Buffer->text[line_idx] = realloc(Buffer->text[line_idx], memblock);

#ifdef DEBUG_MEMORY
        printf("Extend_line %u with mem of %u B.\n",
               line_idx + IDX, memblock);
#endif

    }

    if(Buffer->text[line_idx] == NULL)
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
    if((BUFFER__CURRENT_LINE_LENGTH >= BUFFER__INITIAL_MEMBLOCK)
       && (BUFFER__CURRENT_LINE_LENGTH < BUFFER__MEMBLOCK))
    {
        // Shrink to size of the BUFFER__MEMBLOCK.
        memblock = BUFFER__MEMBLOCK;
    }
    else if(BUFFER__CURRENT_LINE_LENGTH >= BUFFER__MEMBLOCK)
    {
        // Remove the newest memblock because isn't needed now.
        memblock = ((BUFFER__CURRENT_LINE_LENGTH / BUFFER__MEMBLOCK)
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

    if((BUFFER__PREVIOUS_LINE_LENGTH >= BUFFER__INITIAL_MEMBLOCK)
       && (BUFFER__PREVIOUS_LINE_LENGTH < BUFFER__MEMBLOCK))
    {
        memblock = BUFFER__MEMBLOCK;
    }
    else if((BUFFER__PREVIOUS_LINE_LENGTH >= BUFFER__MEMBLOCK))
    {
        // Set the size of some MEMBLKs.
        memblock = ((BUFFER__PREVIOUS_LINE_LENGTH / BUFFER__MEMBLOCK)
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
    Buffer->text = realloc(Buffer->text, (Buffer->lines_idx + IDX)
                           * BUFFER__ADDR_SZ);

    if(Buffer->text == NULL)
    {
        fprintf(stderr, "Can't extend the array with lines.\n");
        return false;
    }

    // Enhance the array that contains lines length indicators.
    Buffer->lines_length = realloc(Buffer->lines_length, BUFFER__ADDR_SZ *
                                   (Buffer->lines_idx + IDX));

    if(Buffer->lines_length == NULL)
    {
        fprintf(stderr, "Can't extend the array with lines length.\n");
        return false;
    }

    // The new line is allocated with only 4 or 8 bytes bytes.
    if((BUFFER__LAST_LINE = malloc(BUFFER__INITIAL_MEMBLOCK)) == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for the new line.\n");
        return false;
    }

    // Naturally the new line doesn't contains any chars - only terminator.
    BUFFER__LAST_LINE_LENGTH = 0;

    return true;
}

bool memory__shrink_lines_array(Buff_t* Buffer)
{
    Buffer->text = realloc(Buffer->text, (Buffer->lines_idx + IDX)
                           * BUFFER__ADDR_SZ);

    if(Buffer->text == NULL)
    {
        fprintf(stderr, "Can't shrink the array with lines.\n");
        return false;
    }

    Buffer->lines_length = realloc(Buffer->lines_length, BUFFER__ADDR_SZ
                                   * (Buffer->lines_idx + IDX));

    if(Buffer->lines_length == NULL)
    {
        fprintf(stderr, "Can't shrink the array with lines length.\n");
        return false;
    }
    return true;
}

bool memory__copy_lines_forward(Buff_t* Buffer)
{
    for(idx_t line_idx = Buffer->lines_idx; line_idx > BUFFER__CURRENT_LINE_IDX;
        line_idx--)
    {
        idx_t memblock = ((Buffer->lines_length[line_idx - PREV]
                         / BUFFER__MEMBLOCK) * BUFFER__MEMBLOCK)
                         + BUFFER__MEMBLOCK;

        Buffer->text[line_idx] = realloc(Buffer->text[line_idx], memblock);

#ifdef DEBUG_MEMORY
        printf("Copied line %u forward to %u with new mem of %u B.\n",
               line_idx + IDX - PREV, line_idx + IDX, memblock);
#endif

        if(Buffer->text[line_idx] == NULL)
        {
            fprintf(stderr, "Can't resize the line forward.\n");
            return false;
        }
        strcpy(Buffer->text[line_idx], Buffer->text[line_idx - PREV]);

        Buffer->lines_length[line_idx] = Buffer->lines_length[line_idx - PREV];
    }
    return true;
}

bool memory__copy_lines_backward(Buff_t* Buffer)
{
    for(idx_t line_idx = BUFFER__CURRENT_LINE_IDX; line_idx < Buffer->lines_idx;
        line_idx++)
    {
        idx_t memblock = ((Buffer->lines_length[line_idx + NEXT]
                         / BUFFER__MEMBLOCK) * BUFFER__MEMBLOCK)
                         + BUFFER__MEMBLOCK;

        Buffer->text[line_idx] = realloc(Buffer->text[line_idx], memblock);

#ifdef DEBUG_MEMORY
        printf("Copied line %u backward to %u with new mem of %u B.\n",
               line_idx + IDX + NEXT, line_idx + IDX, memblock);
#endif

        if(Buffer->text[line_idx] == NULL)
        {
            fprintf(stderr, "Can't copy a line backward.\n");
            return false;
        }
        strcpy(Buffer->text[line_idx], Buffer->text[line_idx + NEXT]);

        Buffer->lines_length[line_idx] = Buffer->lines_length[line_idx + NEXT];
    }
    return true;
}
