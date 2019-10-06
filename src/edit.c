#include "edit.h"

bool edit__delete_char(V_file* v_file)
{
    if(!V_FILE__CURSOR_AT_LINE_START)
    {
        edit__shift_text_horizonally(v_file, 'l');
        if(!memory__shrink_current_line(v_file))
        {
            return false;
        }
        V_FILE__ACTUAL_LINE.len--;
        v_file->chars_amount--;
    }
    // Deletes a non-empty line and copy chars to previous.
    else if(!V_FILE__FIRST_LINE && !edit__move_lines_backward(v_file))
    {
        return false;
    }
    V_FILE__LAST_CHAR_IN_LINE = '\0'; // Delete the linefeed.

    return true;
}

bool edit__delete_line(V_file* v_file)
{
    const size_t next_line_i   = V_FILE__ACTUAL_LINE_I + SIZE__NEXT;
    const size_t next_line_len = v_file->lines[next_line_i].len;

    if(!V_FILE__FIRST_LINE)
    {
        if(V_FILE__CURSOR_Y_SCROLLED)
        {
            v_file->mirrored_cursor_x = (V_FILE__CURSOR_AT_LINE_START)
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
            V_FILE__LAST_LINE.len--;
            V_FILE__LAST_LINE.txt[V_FILE__LAST_LINE.len] = '\0';

            v_file->mirrored_cursor_x = 0;
        }
    }
    else
    {
        V_FILE__LAST_LINE.len                      = 0;
        V_FILE__LAST_LINE.txt[V_FILE__LAST_LINE.len] = '\0';

        V_FILE__LAST_LINE.txt = realloc(V_FILE__LAST_LINE.txt,
                                      V_FILE__BASIC_MEMBLK);
        if(V_FILE__LAST_LINE.txt == NULL)
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
        for(char_i = V_FILE__CURSOR_X; char_i <= V_FILE__ACTUAL_LINE.len; char_i++)
        {
            tmp_char = V_FILE__ACTUAL_LINE.txt[char_i];
            V_FILE__ACTUAL_LINE.txt[char_i - SIZE__PREV] = tmp_char;
        }
        break;

    case 'r':
        for(char_i = V_FILE__ACTUAL_LINE.len; char_i >= V_FILE__CURSOR_X; char_i--)
        {
            tmp_char = V_FILE__ACTUAL_LINE.txt[char_i - SIZE__PREV];
            V_FILE__ACTUAL_LINE.txt[char_i] = tmp_char;
        }
    }
}

bool edit__move_lines_forward(V_file* v_file)
{
    V_FILE__PREV_LINE.len -= v_file->mirrored_cursor_x;

    // Move more lines vertically with a part of a current line.
    if(V_FILE__CURSOR_Y_SCROLLED)
    {
        if(!memory__copy_lines_forward(v_file))
        {
            return false;
        }
        V_FILE__ACTUAL_LINE.len = 0;
    }

    // Move a right part (separated by the cursor) of a line to a next.
    for(size_t char_i = V_FILE__PREV_LINE.len;
        char_i < V_FILE__PREV_LINE.len + v_file->mirrored_cursor_x; char_i++)
    {
        V_FILE__LAST_CHAR_IN_LINE = V_FILE__PREV_LINE.txt[char_i];
        V_FILE__ACTUAL_LINE.len++;

        if(!memory__extend_line(v_file, V_FILE__ACTUAL_LINE_I))
        {
            return false;
        }
    }

    // Now a length of an upper line will be shrinked after copying.
    V_FILE__PREV_LINE.txt[V_FILE__PREV_LINE.len] = '\0';
    if(!memory__shrink_prev_line(v_file))
    {
        return false;
    }
    return true;
}

bool edit__move_lines_backward(V_file* v_file)
{
    v_file->chars_amount--;
    V_FILE__PREV_LINE.len--;

    // Merge a previous line with a next.
    for(size_t char_i = 0; char_i <= V_FILE__ACTUAL_LINE.len; char_i++)
    {
        V_FILE__PREV_LINE.txt[V_FILE__PREV_LINE.len] = V_FILE__ACTUAL_LINE.txt[char_i];

        if(V_FILE__ACTUAL_LINE.txt[char_i] != '\0')
        {
            V_FILE__PREV_LINE.len++;
        }
        if(!memory__extend_line(v_file, V_FILE__PREV_LINE_I))
        {
            return false;
        }
    }
    if(V_FILE__CURSOR_Y_SCROLLED && !memory__copy_lines_backward(v_file))
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
    free(V_FILE__ACTUAL_LINE.txt);

    v_file->lines_amount--;
    if(!memory__shrink_current_line(v_file))
    {
        return false;
    }
    V_FILE__ACTUAL_LINE.len--;
    v_file->chars_amount--;

    if(!memory__shrink_lines_array(v_file))
    {
        return false;
    }
    return true;
}

bool edit__delete_last_line(V_file* v_file)
{
    free(V_FILE__LAST_LINE.txt);

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
