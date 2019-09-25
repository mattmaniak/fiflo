#include "memory.h"

bool memory__extend_line(V_file_t* const V_file, const size_t ln_i)
{
    size_t memblk = V_FILE__MEMBLK;
    size_t ln_len = V_file->Lines[ln_i].len;

    if(ln_len == V_FILE__BASIC_MEMBLK)
    {
        // There are 4/8 (on different archs) chars, extend to V_FILE__MEMBLK.
        V_file->Lines[ln_i].txt = realloc(V_file->Lines[ln_i].txt, memblk);

#ifdef DEBUG_MEMORY
        printf("Extend_line %u with mem of %u B.\n", ln_i + SIZE__I,
               memblk);
#endif

    }
    else if((ln_len > V_FILE__BASIC_MEMBLK) && (ln_len % V_FILE__MEMBLK == 0))
    {
        // There are more chars so append the new memblock.
        memblk = ((ln_len / V_FILE__MEMBLK) * V_FILE__MEMBLK) + V_FILE__MEMBLK;

        V_file->Lines[ln_i].txt = realloc(V_file->Lines[ln_i].txt, memblk);

#ifdef DEBUG_MEMORY
        printf("Extend_line %u with mem of %u B.\n", ln_i + SIZE__I,
               memblk);
#endif

    }
    if(V_file->Lines[ln_i].txt == NULL)
    {
        fprintf(stderr, "Can't extend a memory block for the line %lu.\n",
                ln_i + SIZE__I);
        return false;
    }
    return true;
}

bool memory__shrink_current_line(V_file_t* const V_file)
{
    size_t memblk = V_FILE__BASIC_MEMBLK;

    /* These cases are executed only when the backspace is pressed. Works in
       the same way as "extend_current_line". */
    if((V_FILE__ACTUAL_LN.len >= V_FILE__BASIC_MEMBLK)
       && (V_FILE__ACTUAL_LN.len < V_FILE__MEMBLK))
    {
        // Shrink to size of the V_FILE__MEMBLK.
        memblk = V_FILE__MEMBLK;
    }
    else if(V_FILE__ACTUAL_LN.len >= V_FILE__MEMBLK)
    {
        // Remove the newest memblk because isn't needed now.
        memblk = ((V_FILE__ACTUAL_LN.len / V_FILE__MEMBLK) * V_FILE__MEMBLK)
                 + V_FILE__MEMBLK;
    }

#ifdef DEBUG_MEMORY
    printf("Shrink_current_line %u with mem of %u B.\n",
           V_FILE__ACTUAL_LN_I + SIZE__I, memblk);
#endif

    V_FILE__ACTUAL_LN.txt = realloc(V_FILE__ACTUAL_LN.txt, memblk);
    if(V_FILE__ACTUAL_LN.txt == NULL)
    {
        fprintf(stderr, "Can't shrink a memory block for the line %lu\n",
                V_FILE__ACTUAL_LN_I + SIZE__I);
        return false;
    }
    return true;
}

bool memory__shrink_prev_line(V_file_t* const V_file)
{
    size_t memblk = V_FILE__BASIC_MEMBLK;

    if((V_FILE__PREV_LN.len >= V_FILE__BASIC_MEMBLK)
       && (V_FILE__PREV_LN.len < V_FILE__MEMBLK))
    {
        memblk = V_FILE__MEMBLK;
    }
    else if((V_FILE__PREV_LN.len >= V_FILE__MEMBLK))
    {
        // Set the size of some MEMBLKs.
        memblk = ((V_FILE__PREV_LN.len / V_FILE__MEMBLK) * V_FILE__MEMBLK)
                 + V_FILE__MEMBLK;
    }

#ifdef DEBUG_MEMORY
    printf("Shrink_PREV_line %u with mem of %u B\n",
           V_FILE__PREV_LN_I + SIZE__I, memblk);
#endif

    V_FILE__PREV_LN.txt = realloc(V_FILE__PREV_LN.txt, memblk);
    if(V_FILE__PREV_LN.txt == NULL)
    {
        fprintf(stderr, "Can't shrink a memory block for the prev. %lu line.\n",
                V_FILE__PREV_LN_I + SIZE__I);
        return false;
    }
    return true;
}

bool memory__extend_lines_array(V_file_t* const V_file)
{
    // Enhance the array that contains pointers to lines.
    V_file->Lines = realloc(V_file->Lines,
                            (V_file->ln_amount + SIZE__I) * sizeof(Line_t));
    if(V_file->Lines == NULL)
    {
        fprintf(stderr, "Can't extend an array with lines.\n");
        return false;
    }

    // The new line is allocated with only 4 or 8 bytes bytes.
    V_FILE__LAST_LN.txt = malloc(V_FILE__BASIC_MEMBLK);
    if(V_FILE__LAST_LN.txt == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for a new line.\n");
        return false;
    }
    // Naturally the new line doesn't contains any chars - only terminator.
    V_FILE__LAST_LN.len = 0;

    return true;
}

bool memory__shrink_lines_array(V_file_t* const V_file)
{
    V_file->Lines = realloc(V_file->Lines,
                            (V_file->ln_amount + SIZE__I) * sizeof(Line_t));
    if(V_file->Lines == NULL)
    {
        fprintf(stderr, "Can't shrink an array with lines.\n");
        return false;
    }
    return true;
}

bool memory__copy_lines_forward(V_file_t* const V_file)
{
    for(size_t ln_i = V_file->ln_amount; ln_i > V_FILE__ACTUAL_LN_I; ln_i--)
    {
        size_t memblk = ((V_file->Lines[ln_i - SIZE__PREV].len
                         / V_FILE__MEMBLK) * V_FILE__MEMBLK) + V_FILE__MEMBLK;

        V_file->Lines[ln_i].txt = realloc(V_file->Lines[ln_i].txt, memblk);

#ifdef DEBUG_MEMORY
        printf("Copied line %u forward to %u with new mem of %u B.\n",
               ln_i + SIZE__I - SIZE__PREV, ln_i + SIZE__I, memblk);
#endif

        if(V_file->Lines[ln_i].txt == NULL)
        {
            fprintf(stderr, "Can't resize the forward %lu line.\n",
                    ln_i + SIZE__I);
            return false;
        }
        strcpy(V_file->Lines[ln_i].txt, V_file->Lines[ln_i - SIZE__PREV].txt);
        V_file->Lines[ln_i].len = V_file->Lines[ln_i - SIZE__PREV].len;
    }
    return true;
}

bool memory__copy_lines_backward(V_file_t* const V_file)
{
    for(size_t ln_i = V_FILE__ACTUAL_LN_I; ln_i < V_file->ln_amount; ln_i++)
    {
        size_t memblk = ((V_file->Lines[ln_i + SIZE__NEXT].len
                         / V_FILE__MEMBLK) * V_FILE__MEMBLK) + V_FILE__MEMBLK;

        V_file->Lines[ln_i].txt = realloc(V_file->Lines[ln_i].txt, memblk);

#ifdef DEBUG_MEMORY
        printf("Copied line %u backward to %u with new mem of %u B.\n",
               ln_i + SIZE__I + SIZE__NEXT, ln_i + SIZE__I, memblk);
#endif

        if(V_file->Lines[ln_i].txt == NULL)
        {
            fprintf(stderr, "Can't copy the line %lu backward.\n",
                    ln_i + SIZE__I);
            return false;
        }
        strcpy(V_file->Lines[ln_i].txt, V_file->Lines[ln_i + SIZE__NEXT].txt);
        V_file->Lines[ln_i].len = V_file->Lines[ln_i + SIZE__NEXT].len;
    }
    return true;
}
