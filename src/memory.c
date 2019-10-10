#include "memory.h"

bool memory__extend_line(V_file* const v_file, const size_t line_i)
{
    size_t memblock = V_FILE__MEMBLOCK;
    size_t line_len = v_file->lines[line_i].length;

    if(line_len == V_FILE__BASIC_MEMBLOCK)
    {
        // There are 4/8 (on different arch) chars, extend to V_FILE__MEMBLOCK.
        v_file->lines[line_i].txt = realloc(v_file->lines[line_i].txt,
                                            memblock);

#ifdef DEBUG_MEMORY
        printf("Extend_line %u with mem of %u B.\n", line_i + SIZE__I,
               memblock);
#endif

    }
    else if((line_len > V_FILE__BASIC_MEMBLOCK)
            && (line_len % V_FILE__MEMBLOCK == 0))
    {
        // There are more chars so append the new memblock.
        memblock = ((line_len / V_FILE__MEMBLOCK) * V_FILE__MEMBLOCK)
                   + V_FILE__MEMBLOCK;

        v_file->lines[line_i].txt = realloc(v_file->lines[line_i].txt,
                                            memblock);

#ifdef DEBUG_MEMORY
        printf("Extend_line %u with mem of %u B.\n", line_i + SIZE__I,
               memblock);
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
    size_t memblock = V_FILE__BASIC_MEMBLOCK;

    /* These cases are executed only when the backspace is pressed. Works in
       the same way as "extend_current_line". */
    if((v_file_actual_line(v_file)->length >= V_FILE__BASIC_MEMBLOCK)
       && (v_file_actual_line(v_file)->length < V_FILE__MEMBLOCK))
    {
        // Shrink to size of the V_FILE__MEMBLOCK.
        memblock = V_FILE__MEMBLOCK;
    }
    else if(v_file_actual_line(v_file)->length >= V_FILE__MEMBLOCK)
    {
        // Remove the newest memblock because isn't needed now.
        memblock = ((v_file_actual_line(v_file)->length / V_FILE__MEMBLOCK)
                    * V_FILE__MEMBLOCK) + V_FILE__MEMBLOCK;
    }

#ifdef DEBUG_MEMORY
    printf("Shrink_current_line %u with mem of %u B.\n",
           v_file_cursor_y(v_file) + SIZE__I, memblock);
#endif

    v_file_actual_line(v_file)->txt
        = realloc(v_file_actual_line(v_file)->txt, memblock);
    if(v_file_actual_line(v_file)->txt == NULL)
    {
        fprintf(stderr, "Can't shrink a memory block for the line %u\n",
                (int) v_file_cursor_y(v_file) + SIZE__I);
        return false;
    }
    return true;
}

bool memory__shrink_prev_line(V_file* const v_file)
{
    const size_t prev_line_number = v_file_cursor_y(v_file) - SIZE__PREV
                                    + SIZE__I;

    size_t memblock = V_FILE__BASIC_MEMBLOCK;

    if((v_file_prev_line(v_file)->length >= V_FILE__BASIC_MEMBLOCK)
       && (v_file_prev_line(v_file)->length < V_FILE__MEMBLOCK))
    {
        memblock = V_FILE__MEMBLOCK;
    }
    else if((v_file_prev_line(v_file)->length >= V_FILE__MEMBLOCK))
    {
        // Set the size of some MEMBLOCKs.
        memblock = ((v_file_prev_line(v_file)->length / V_FILE__MEMBLOCK)
                    * V_FILE__MEMBLOCK) + V_FILE__MEMBLOCK;
    }

#ifdef DEBUG_MEMORY
    printf("Shrink_PREV_line %u with mem of %u B\n", prev_line_number,
           memblock);
#endif

    v_file_prev_line(v_file)->txt
        = realloc(v_file_prev_line(v_file)->txt, memblock);
    if(v_file_prev_line(v_file)->txt == NULL)
    {
        fprintf(stderr, "Can't shrink a memory block for the prev. %u line.\n",
                (unsigned) prev_line_number);
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
    v_file_last_line(v_file)->txt = malloc(V_FILE__BASIC_MEMBLOCK);
    if(v_file_last_line(v_file)->txt == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for a new line.\n");
        return false;
    }
    // Naturally the new line doesn't contains any chars - only terminator.
    v_file_last_line(v_file)->length = 0;

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
    for(size_t line_i = v_file->lines_amount;
        line_i > v_file_cursor_y(v_file); line_i--)
    {
        size_t memblock = ((v_file->lines[line_i - SIZE__PREV].length
                           / V_FILE__MEMBLOCK) * V_FILE__MEMBLOCK)
                          + V_FILE__MEMBLOCK;

        v_file->lines[line_i].txt = realloc(v_file->lines[line_i].txt,
                                            memblock);

#ifdef DEBUG_MEMORY
        printf("Copied line %u forward to %u with new mem of %u B.\n",
               line_i + SIZE__I - SIZE__PREV, line_i + SIZE__I, memblock);
#endif

        if(v_file->lines[line_i].txt == NULL)
        {
            fprintf(stderr, "Can't resize the forward %u line.\n",
                    (unsigned) line_i + SIZE__I);
            return false;
        }
        strcpy(v_file->lines[line_i].txt,
               v_file->lines[line_i - SIZE__PREV].txt);

        v_file->lines[line_i].length = v_file->lines[
                                           line_i - SIZE__PREV].length;
    }
    return true;
}

bool memory__copy_lines_backward(V_file* const v_file)
{
    for(size_t line_i = v_file_cursor_y(v_file); line_i < v_file->lines_amount;
        line_i++)
    {
        size_t memblock = ((v_file->lines[line_i + SIZE__NEXT].length
                           / V_FILE__MEMBLOCK) * V_FILE__MEMBLOCK)
                          + V_FILE__MEMBLOCK;

        v_file->lines[line_i].txt = realloc(v_file->lines[line_i].txt,
                                            memblock);

#ifdef DEBUG_MEMORY
        printf("Copied line %u backward to %u with new mem of %u B.\n",
               line_i + SIZE__I + SIZE__NEXT, line_i + SIZE__I, memblock);
#endif

        if(v_file->lines[line_i].txt == NULL)
        {
            fprintf(stderr, "Can't copy the line %u backward.\n",
                    (unsigned) line_i + SIZE__I);
            return false;
        }
        strcpy(v_file->lines[line_i].txt,
               v_file->lines[line_i + SIZE__NEXT].txt);

        v_file->lines[line_i].length = v_file->lines[
                                           line_i + SIZE__NEXT].length;
    }
    return true;
}
