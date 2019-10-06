#include "memory.h"

bool memory__extend_line(V_file* const v_file, const size_t line_i)
{
    size_t memblk = V_FILE__MEMBLK;
    size_t line_len = v_file->lines[line_i].len;

    if(line_len == V_FILE__BASIC_MEMBLK)
    {
        // There are 4/8 (on different archs) chars, extend to V_FILE__MEMBLK.
        v_file->lines[line_i].txt = realloc(v_file->lines[line_i].txt, memblk);

#ifdef DEBUG_MEMORY
        printf("Extend_line %u with mem of %u B.\n", line_i + SIZE__I, memblk);
#endif

    }
    else if((line_len > V_FILE__BASIC_MEMBLK) && (line_len % V_FILE__MEMBLK == 0))
    {
        // There are more chars so append the new memblock.
        memblk = ((line_len / V_FILE__MEMBLK) * V_FILE__MEMBLK) + V_FILE__MEMBLK;

        v_file->lines[line_i].txt = realloc(v_file->lines[line_i].txt, memblk);

#ifdef DEBUG_MEMORY
        printf("Extend_line %u with mem of %u B.\n", line_i + SIZE__I, memblk);
#endif

    }
    if(v_file->lines[line_i].txt == NULL)
    {
        fprintf(stderr, "Can't extend a memory block for the line %u.\n",
                (unsigned) line_i + SIZE__I);
        return false;
    }
    return true;
}

bool memory__shrink_current_line(V_file* const v_file)
{
    size_t memblk = V_FILE__BASIC_MEMBLK;

    /* These cases are executed only when the backspace is pressed. Works in
       the same way as "extend_current_line". */
    if((V_FILE__ACTUAL_LINE.len >= V_FILE__BASIC_MEMBLK)
       && (V_FILE__ACTUAL_LINE.len < V_FILE__MEMBLK))
    {
        // Shrink to size of the V_FILE__MEMBLK.
        memblk = V_FILE__MEMBLK;
    }
    else if(V_FILE__ACTUAL_LINE.len >= V_FILE__MEMBLK)
    {
        // Remove the newest memblk because isn't needed now.
        memblk = ((V_FILE__ACTUAL_LINE.len / V_FILE__MEMBLK) * V_FILE__MEMBLK)
                 + V_FILE__MEMBLK;
    }

#ifdef DEBUG_MEMORY
    printf("Shrink_current_line %u with mem of %u B.\n",
           V_FILE__ACTUAL_LINE_I + SIZE__I, memblk);
#endif

    V_FILE__ACTUAL_LINE.txt = realloc(V_FILE__ACTUAL_LINE.txt, memblk);
    if(V_FILE__ACTUAL_LINE.txt == NULL)
    {
        fprintf(stderr, "Can't shrink a memory block for the line %u\n",
                (int) V_FILE__ACTUAL_LINE_I + SIZE__I);
        return false;
    }
    return true;
}

bool memory__shrink_prev_line(V_file* const v_file)
{
    size_t memblk = V_FILE__BASIC_MEMBLK;

    if((V_FILE__PREV_LINE.len >= V_FILE__BASIC_MEMBLK)
       && (V_FILE__PREV_LINE.len < V_FILE__MEMBLK))
    {
        memblk = V_FILE__MEMBLK;
    }
    else if((V_FILE__PREV_LINE.len >= V_FILE__MEMBLK))
    {
        // Set the size of some MEMBLKs.
        memblk = ((V_FILE__PREV_LINE.len / V_FILE__MEMBLK) * V_FILE__MEMBLK)
                 + V_FILE__MEMBLK;
    }

#ifdef DEBUG_MEMORY
    printf("Shrink_PREV_line %u with mem of %u B\n",
           V_FILE__PREV_LINE_I + SIZE__I, memblk);
#endif

    V_FILE__PREV_LINE.txt = realloc(V_FILE__PREV_LINE.txt, memblk);
    if(V_FILE__PREV_LINE.txt == NULL)
    {
        fprintf(stderr, "Can't shrink a memory block for the prev. %u line.\n",
                (unsigned) V_FILE__PREV_LINE_I + SIZE__I);
        return false;
    }
    return true;
}

bool memory__extend_lines_array(V_file* const v_file)
{
    // Enhance the array that contains pointers to lines.
    v_file->lines = realloc(v_file->lines,
                            (v_file->lines_amount + SIZE__I) * sizeof(Line));
    if(v_file->lines == NULL)
    {
        fprintf(stderr, "Can't extend an array with lines.\n");
        return false;
    }

    // The new line is allocated with only 4 or 8 bytes bytes.
    V_FILE__LAST_LINE.txt = malloc(V_FILE__BASIC_MEMBLK);
    if(V_FILE__LAST_LINE.txt == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for a new line.\n");
        return false;
    }
    // Naturally the new line doesn't contains any chars - only terminator.
    V_FILE__LAST_LINE.len = 0;

    return true;
}

bool memory__shrink_lines_array(V_file* const v_file)
{
    v_file->lines = realloc(v_file->lines,
                            (v_file->lines_amount + SIZE__I) * sizeof(Line));
    if(v_file->lines == NULL)
    {
        fprintf(stderr, "Can't shrink an array with lines.\n");
        return false;
    }
    return true;
}

bool memory__copy_lines_forward(V_file* const v_file)
{
    for(size_t line_i = v_file->lines_amount; line_i > V_FILE__ACTUAL_LINE_I; line_i--)
    {
        size_t memblk = ((v_file->lines[line_i - SIZE__PREV].len
                         / V_FILE__MEMBLK) * V_FILE__MEMBLK) + V_FILE__MEMBLK;

        v_file->lines[line_i].txt = realloc(v_file->lines[line_i].txt, memblk);

#ifdef DEBUG_MEMORY
        printf("Copied line %u forward to %u with new mem of %u B.\n",
               line_i + SIZE__I - SIZE__PREV, line_i + SIZE__I, memblk);
#endif

        if(v_file->lines[line_i].txt == NULL)
        {
            fprintf(stderr, "Can't resize the forward %u line.\n",
                    (unsigned) line_i + SIZE__I);
            return false;
        }
        strcpy(v_file->lines[line_i].txt, v_file->lines[line_i - SIZE__PREV].txt);
        v_file->lines[line_i].len = v_file->lines[line_i - SIZE__PREV].len;
    }
    return true;
}

bool memory__copy_lines_backward(V_file* const v_file)
{
    for(size_t line_i = V_FILE__ACTUAL_LINE_I; line_i < v_file->lines_amount; line_i++)
    {
        size_t memblk = ((v_file->lines[line_i + SIZE__NEXT].len
                         / V_FILE__MEMBLK) * V_FILE__MEMBLK) + V_FILE__MEMBLK;

        v_file->lines[line_i].txt = realloc(v_file->lines[line_i].txt, memblk);

#ifdef DEBUG_MEMORY
        printf("Copied line %u backward to %u with new mem of %u B.\n",
               line_i + SIZE__I + SIZE__NEXT, line_i + SIZE__I, memblk);
#endif

        if(v_file->lines[line_i].txt == NULL)
        {
            fprintf(stderr, "Can't copy the line %u backward.\n",
                    (unsigned) line_i + SIZE__I);
            return false;
        }
        strcpy(v_file->lines[line_i].txt, v_file->lines[line_i + SIZE__NEXT].txt);
        v_file->lines[line_i].len = v_file->lines[line_i + SIZE__NEXT].len;
    }
    return true;
}
