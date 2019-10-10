#include "edit.h"

bool edit__delete_char(V_file* v_file)
{
    if(!v_file__is_cursor_at_line_start(v_file))
    {
        edit__shift_text_horizonally(v_file, 'l');
        if(!memory__shrink_current_line(v_file))
        {
            return false;
        }
        v_file_actual_line(v_file)->length--;
        v_file->chars_amount--;
    }
    // Deletes a non-empty line and copy chars to previous.
    else if(!v_file__is_actual_line_first(v_file)
            && !edit__move_lines_backward(v_file))
    {
        return false;
    }
    // Delete the LF.
    v_file_actual_line(v_file)->txt[
        v_file_actual_line(v_file)->length] = '\0';

    return true;
}

bool edit__delete_line(V_file* v_file)
{
    const size_t next_line_i   = v_file_cursor_y(v_file) + SIZE__NEXT;
    const size_t next_line_len = v_file->lines[next_line_i].length;
    bool         cursor_is_at_line_start;

    cursor_is_at_line_start = v_file__is_cursor_at_line_start(v_file);

    if(!v_file__is_actual_line_first(v_file))
    {
        if(v_file__is_cursor_y_scrolled(v_file))
        {
            v_file->mirrored_cursor_x = (cursor_is_at_line_start)
                                        ? next_line_len : SIZE__LF;

            if(!memory__copy_lines_backward(v_file)
               || !edit__delete_last_line(v_file))
            {
                return false;
            }
            v_file->mirrored_cursor_y--;
        }
        else
        {
            if(!edit__delete_last_line(v_file))
            {
                return false;
            }

            /* With a last line deletion there is a need to remove the
               linefeed in a previous line. */
            v_file_last_line(v_file)->length--;
            v_file_last_line(v_file)->txt[v_file_last_line(v_file)->length]
                = '\0';

            v_file->mirrored_cursor_x = 0;
        }
    }
    else
    {
        v_file_last_line(v_file)->length                                = 0;
        v_file_last_line(v_file)->txt[v_file_last_line(v_file)->length] = '\0';

        v_file_last_line(v_file)->txt = realloc(v_file_last_line(v_file)->txt,
                                                V_FILE__BASIC_MEMBLOCK);
        if(v_file_last_line(v_file)->txt == NULL)
        {
            fprintf(stderr, "Can't realloc a memory in a first line.\n");
        }
        v_file->mirrored_cursor_x = 0;
    }
    return true;
}

void edit__shift_text_horizonally(V_file* v_file, const char direction)
{
    size_t char_i;
    char   tmp_char;

    switch(direction)
    {
    case 'l':
        for(char_i = v_file_cursor_x(v_file);
            char_i <= v_file_actual_line(v_file)->length; char_i++)
        {
            tmp_char = v_file_actual_line(v_file)->txt[char_i];
            v_file_actual_line(v_file)->txt[char_i - SIZE__PREV] = tmp_char;
        }
        break;

    case 'r':
        for(char_i = v_file_actual_line(v_file)->length;
            char_i >= v_file_cursor_x(v_file); char_i--)
        {
            tmp_char = v_file_actual_line(v_file)->txt[char_i - SIZE__PREV];
            v_file_actual_line(v_file)->txt[char_i] = tmp_char;
        }
    }
}

bool edit__move_lines_forward(V_file* v_file)
{
    v_file_prev_line(v_file)->length -= v_file->mirrored_cursor_x;

    // Move more lines vertically with a part of a current line.
    if(v_file__is_cursor_y_scrolled(v_file))
    {
        if(!memory__copy_lines_forward(v_file))
        {
            return false;
        }
        v_file_actual_line(v_file)->length = 0;
    }

    // Move a right part (separated by the cursor) of a line to a next.
    for(size_t char_i = v_file_prev_line(v_file)->length;
        char_i < v_file_prev_line(v_file)->length
                 + v_file->mirrored_cursor_x;
        char_i++)
    {
        v_file_actual_line(v_file)->txt[
            v_file_actual_line(v_file)->length]
            = v_file_prev_line(v_file)->txt[char_i];

        v_file_actual_line(v_file)->length++;

        if(!memory__extend_line(v_file, v_file_cursor_y(v_file)))
        {
            return false;
        }
    }

    // Now a length of an upper line will be shrinked after copying.
    v_file_prev_line(v_file)->txt[v_file_prev_line(v_file)->length] = '\0';
    if(!memory__shrink_prev_line(v_file))
    {
        return false;
    }
    return true;
}

bool edit__move_lines_backward(V_file* v_file)
{
    const size_t prev_line_i = v_file_cursor_y(v_file) - SIZE__PREV;

    v_file->chars_amount--;
    v_file_prev_line(v_file)->length--;

    // Merge a previous line with a next.
    for(size_t char_i = 0; char_i <= v_file_actual_line(v_file)->length;
        char_i++)
    {
        v_file_prev_line(v_file)->txt[v_file_prev_line(v_file)->length]
            = v_file_actual_line(v_file)->txt[char_i];

        if(v_file_actual_line(v_file)->txt[char_i] != '\0')
        {
            v_file_prev_line(v_file)->length++;
        }
        if(!memory__extend_line(v_file, prev_line_i))
        {
            return false;
        }
    }
    if(v_file__is_cursor_y_scrolled(v_file)
       && !memory__copy_lines_backward(v_file))
    {
        return false;
    }
    if(!edit__delete_last_line(v_file))
    {
        return false;
    }
    return true;
}

bool edit__delete_last_empty_line(V_file* v_file)
{
    free(v_file_actual_line(v_file)->txt);

    v_file->lines_amount--;
    if(!memory__shrink_current_line(v_file))
    {
        return false;
    }
    v_file_actual_line(v_file)->length--;
    v_file->chars_amount--;

    if(!memory__shrink_lines_array(v_file))
    {
        return false;
    }
    return true;
}

bool edit__delete_last_line(V_file* v_file)
{
    free(v_file_last_line(v_file)->txt);

    v_file->lines_amount--;
    return memory__shrink_lines_array(v_file);
}

void edit__filename(V_file* v_file, const Config* const config, Modes* modes,
                    const char key)
{
    const char escape = ASCII__CTRL_LEFT_BRACKET;

    if((key >= 32) && (key != ASCII__BACKSPACE)
       && ((v_file->fname_len + SIZE__I) < PATH_MAX))
    {
        v_file->fname[v_file->fname_len]   = key;
        v_file->fname[++v_file->fname_len] = '\0';
    }
    else if((key == ASCII__BACKSPACE) && (v_file->fname_len > 0))
    {
        v_file->fname[--v_file->fname_len] = '\0';
    }
    else if(key == '\n')
    {
        modes->live_fname_edit = false;
        V_FILE__SET_STATUS("saved as with a different name");

        if(!strcmp(v_file->fname, v_file->fname_copy))
        {
            V_FILE__SET_STATUS("saved as");
        }
        file_io__save(v_file, config);
        strncpy(v_file->fname_copy, v_file->fname, PATH_MAX);
    }
    else if(key == escape)
    {
        strncpy(v_file->fname, v_file->fname_copy, PATH_MAX);
        v_file->fname_len = strlen(v_file->fname);

        modes->live_fname_edit = false;
        V_FILE__SET_STATUS("filename unchanged");
    }
}
