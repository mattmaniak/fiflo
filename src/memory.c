#include "memory.h"

bool memory__extend_line(Buff_t* const Buffer, const size_t ln_i)
{
    size_t memblk = BUFFER__MEMBLK;
    size_t ln_len = Buffer->Lines[ln_i].len;

    if(ln_len == BUFFER__BASIC_MEMBLK)
    {
        // There are 4/8 (on different archs) chars, extend to BUFFER__MEMBLK.
        Buffer->Lines[ln_i].txt = realloc(Buffer->Lines[ln_i].txt, memblk);

#ifdef DEBUG_MEMORY
        printf("Extend_line %u with mem of %u B.\n", ln_i + SIZE__I,
               memblk);
#endif

    }
    else if((ln_len > BUFFER__BASIC_MEMBLK) && (ln_len % BUFFER__MEMBLK == 0))
    {
        // There are more chars so append the new memblock.
        memblk = ((ln_len / BUFFER__MEMBLK) * BUFFER__MEMBLK) + BUFFER__MEMBLK;

        Buffer->Lines[ln_i].txt = realloc(Buffer->Lines[ln_i].txt, memblk);

#ifdef DEBUG_MEMORY
        printf("Extend_line %u with mem of %u B.\n", ln_i + SIZE__I,
               memblk);
#endif

    }
    if(Buffer->Lines[ln_i].txt == NULL)
    {
        fprintf(stderr, "Can't extend a memory block for the line %lu.\n",
                ln_i + SIZE__I);
        return false;
    }
    return true;
}

bool memory__shrink_current_line(Buff_t* const Buffer)
{
    size_t memblk = BUFFER__BASIC_MEMBLK;

    /* These cases are executed only when the backspace is pressed. Works in
       the same way as "extend_current_line". */
    if((BUFFER__ACTUAL_LN.len >= BUFFER__BASIC_MEMBLK)
       && (BUFFER__ACTUAL_LN.len < BUFFER__MEMBLK))
    {
        // Shrink to size of the BUFFER__MEMBLK.
        memblk = BUFFER__MEMBLK;
    }
    else if(BUFFER__ACTUAL_LN.len >= BUFFER__MEMBLK)
    {
        // Remove the newest memblk because isn't needed now.
        memblk = ((BUFFER__ACTUAL_LN.len / BUFFER__MEMBLK) * BUFFER__MEMBLK)
                 + BUFFER__MEMBLK;
    }

#ifdef DEBUG_MEMORY
    printf("Shrink_current_line %u with mem of %u B.\n",
           BUFFER__ACTUAL_LN_I + SIZE__I, memblk);
#endif

    BUFFER__ACTUAL_LN.txt = realloc(BUFFER__ACTUAL_LN.txt, memblk);
    if(BUFFER__ACTUAL_LN.txt == NULL)
    {
        fprintf(stderr, "Can't shrink a memory block for the line %lu\n",
                BUFFER__ACTUAL_LN_I + SIZE__I);
        return false;
    }
    return true;
}

bool memory__shrink_prev_line(Buff_t* const Buffer)
{
    size_t memblk = BUFFER__BASIC_MEMBLK;

    if((BUFFER__PREV_LN.len >= BUFFER__BASIC_MEMBLK)
       && (BUFFER__PREV_LN.len < BUFFER__MEMBLK))
    {
        memblk = BUFFER__MEMBLK;
    }
    else if((BUFFER__PREV_LN.len >= BUFFER__MEMBLK))
    {
        // Set the size of some MEMBLKs.
        memblk = ((BUFFER__PREV_LN.len / BUFFER__MEMBLK) * BUFFER__MEMBLK)
                 + BUFFER__MEMBLK;
    }

#ifdef DEBUG_MEMORY
    printf("Shrink_PREV_line %u with mem of %u B\n",
           BUFFER__PREV_LN_I + SIZE__I, memblk);
#endif

    BUFFER__PREV_LN.txt = realloc(BUFFER__PREV_LN.txt, memblk);
    if(BUFFER__PREV_LN.txt == NULL)
    {
        fprintf(stderr, "Can't shrink a memory block for the prev. %lu line.\n",
                BUFFER__PREV_LN_I + SIZE__I);
        return false;
    }
    return true;
}

bool memory__extend_lines_array(Buff_t* const Buffer)
{
    // Enhance the array that contains pointers to lines.
    Buffer->Lines = realloc(Buffer->Lines,
                            (Buffer->ln_amount + SIZE__I) * sizeof(Line_t));
    if(Buffer->Lines == NULL)
    {
        fprintf(stderr, "Can't extend an array with lines.\n");
        return false;
    }

    // The new line is allocated with only 4 or 8 bytes bytes.
    BUFFER__LAST_LN.txt = malloc(BUFFER__BASIC_MEMBLK);
    if(BUFFER__LAST_LN.txt == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for a new line.\n");
        return false;
    }
    // Naturally the new line doesn't contains any chars - only terminator.
    BUFFER__LAST_LN.len = 0;

    return true;
}

bool memory__shrink_lines_array(Buff_t* const Buffer)
{
    Buffer->Lines = realloc(Buffer->Lines,
                            (Buffer->ln_amount + SIZE__I) * sizeof(Line_t));
    if(Buffer->Lines == NULL)
    {
        fprintf(stderr, "Can't shrink an array with lines.\n");
        return false;
    }
    return true;
}

bool memory__copy_lines_forward(Buff_t* const Buffer)
{
    for(size_t ln_i = Buffer->ln_amount; ln_i > BUFFER__ACTUAL_LN_I; ln_i--)
    {
        size_t memblk = ((Buffer->Lines[ln_i - SIZE__PREV].len
                         / BUFFER__MEMBLK) * BUFFER__MEMBLK) + BUFFER__MEMBLK;

        Buffer->Lines[ln_i].txt = realloc(Buffer->Lines[ln_i].txt, memblk);

#ifdef DEBUG_MEMORY
        printf("Copied line %u forward to %u with new mem of %u B.\n",
               ln_i + SIZE__I - SIZE__PREV, ln_i + SIZE__I, memblk);
#endif

        if(Buffer->Lines[ln_i].txt == NULL)
        {
            fprintf(stderr, "Can't resize the forward %lu line.\n",
                    ln_i + SIZE__I);
            return false;
        }
        strcpy(Buffer->Lines[ln_i].txt, Buffer->Lines[ln_i - SIZE__PREV].txt);
        Buffer->Lines[ln_i].len = Buffer->Lines[ln_i - SIZE__PREV].len;
    }
    return true;
}

bool memory__copy_lines_backward(Buff_t* const Buffer)
{
    for(size_t ln_i = BUFFER__ACTUAL_LN_I; ln_i < Buffer->ln_amount; ln_i++)
    {
        size_t memblk = ((Buffer->Lines[ln_i + SIZE__NEXT].len
                         / BUFFER__MEMBLK) * BUFFER__MEMBLK) + BUFFER__MEMBLK;

        Buffer->Lines[ln_i].txt = realloc(Buffer->Lines[ln_i].txt, memblk);

#ifdef DEBUG_MEMORY
        printf("Copied line %u backward to %u with new mem of %u B.\n",
               ln_i + SIZE__I + SIZE__NEXT, ln_i + SIZE__I, memblk);
#endif

        if(Buffer->Lines[ln_i].txt == NULL)
        {
            fprintf(stderr, "Can't copy the line %lu backward.\n",
                    ln_i + SIZE__I);
            return false;
        }
        strcpy(Buffer->Lines[ln_i].txt, Buffer->Lines[ln_i + SIZE__NEXT].txt);
        Buffer->Lines[ln_i].len = Buffer->Lines[ln_i + SIZE__NEXT].len;
    }
    return true;
}
