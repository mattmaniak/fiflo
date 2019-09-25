#include "edit.h"

bool edit__delete_char(V_file_t* V_file)
{
    if(!V_FILE__CURSOR_AT_LN_START)
    {
        edit__shift_text_horizonally(V_file, 'l');
        if(!memory__shrink_current_line(V_file))
        {
            return false;
        }
        V_FILE__ACTUAL_LN.len--;
        V_file->ch_amount--;
    }
    // Deletes a non-empty line and copy chars to previous.
    else if(!V_FILE__FIRST_LN && !edit__move_lines_backward(V_file))
    {
        return false;
    }
    V_FILE__LAST_CH_IN_LN = '\0'; // Delete the linefeed.

    return true;
}

bool edit__delete_line(V_file_t* V_file)
{
    const size_t next_line_i   = V_FILE__ACTUAL_LN_I + SIZE__NEXT;
    const size_t next_line_len = V_file->Lines[next_line_i].len;

    if(!V_FILE__FIRST_LN)
    {
        if(V_FILE__CURSOR_Y_SCROLLED)
        {
            V_file->cursor_rev_x = (V_FILE__CURSOR_AT_LN_START)
                                   ? next_line_len : SIZE__LF;

            if(!memory__copy_lines_backward(V_file)
               || !edit__delete_last_line(V_file))
            {
                return false;
            }
            V_file->cursor_rev_y--;
        }
        else
        {
            if(!edit__delete_last_line(V_file))
            {
                return false;
            }

            /* With a last line deletion there is a need to remove the
               linefeed in a previous line. */
            V_FILE__LAST_LN.len--;
            V_FILE__LAST_LN.txt[V_FILE__LAST_LN.len] = '\0';

            V_file->cursor_rev_x = 0;
        }
    }
    else
    {
        V_FILE__LAST_LN.len                      = 0;
        V_FILE__LAST_LN.txt[V_FILE__LAST_LN.len] = '\0';

        V_FILE__LAST_LN.txt = realloc(V_FILE__LAST_LN.txt,
                                      V_FILE__BASIC_MEMBLK);
        if(V_FILE__LAST_LN.txt == NULL)
        {
            fprintf(stderr, "Can't realloc a memory in a first line.\n");
        }
        V_file->cursor_rev_x = 0;
    }
    return true;
}

void edit__shift_text_horizonally(V_file_t* V_file, const char direction)
{
    size_t ch_i;
    char   tmp_ch;

    switch(direction)
    {
    case 'l':
        for(ch_i = V_FILE__CURSOR_X; ch_i <= V_FILE__ACTUAL_LN.len; ch_i++)
        {
            tmp_ch = V_FILE__ACTUAL_LN.txt[ch_i];
            V_FILE__ACTUAL_LN.txt[ch_i - SIZE__PREV] = tmp_ch;
        }
        break;

    case 'r':
        for(ch_i = V_FILE__ACTUAL_LN.len; ch_i >= V_FILE__CURSOR_X; ch_i--)
        {
            tmp_ch = V_FILE__ACTUAL_LN.txt[ch_i - SIZE__PREV];
            V_FILE__ACTUAL_LN.txt[ch_i] = tmp_ch;
        }
    }
}

bool edit__move_lines_forward(V_file_t* V_file)
{
    V_FILE__PREV_LN.len -= V_file->cursor_rev_x;

    // Move more lines vertically with a part of a current line.
    if(V_FILE__CURSOR_Y_SCROLLED)
    {
        if(!memory__copy_lines_forward(V_file))
        {
            return false;
        }
        V_FILE__ACTUAL_LN.len = 0;
    }

    // Move a right part (separated by the cursor) of a line to a next.
    for(size_t ch_i = V_FILE__PREV_LN.len;
        ch_i < V_FILE__PREV_LN.len + V_file->cursor_rev_x; ch_i++)
    {
        V_FILE__LAST_CH_IN_LN = V_FILE__PREV_LN.txt[ch_i];
        V_FILE__ACTUAL_LN.len++;

        if(!memory__extend_line(V_file, V_FILE__ACTUAL_LN_I))
        {
            return false;
        }
    }

    // Now a length of an upper line will be shrinked after copying.
    V_FILE__PREV_LN.txt[V_FILE__PREV_LN.len] = '\0';
    if(!memory__shrink_prev_line(V_file))
    {
        return false;
    }
    return true;
}

bool edit__move_lines_backward(V_file_t* V_file)
{
    V_file->ch_amount--;
    V_FILE__PREV_LN.len--;

    // Merge a previous line with a next.
    for(size_t ch_i = 0; ch_i <= V_FILE__ACTUAL_LN.len; ch_i++)
    {
        V_FILE__PREV_LN.txt[V_FILE__PREV_LN.len] = V_FILE__ACTUAL_LN.txt[ch_i];

        if(V_FILE__ACTUAL_LN.txt[ch_i] != '\0')
        {
            V_FILE__PREV_LN.len++;
        }
        if(!memory__extend_line(V_file, V_FILE__PREV_LN_I))
        {
            return false;
        }
    }
    if(V_FILE__CURSOR_Y_SCROLLED && !memory__copy_lines_backward(V_file))
    {
        return false;
    }
    if(!edit__delete_last_line(V_file))
    {
        return false;
    }
    return true;
}

bool edit__delete_last_empty_line(V_file_t* V_file)
{
    free(V_FILE__ACTUAL_LN.txt);

    V_file->ln_amount--;
    if(!memory__shrink_current_line(V_file))
    {
        return false;
    }
    V_FILE__ACTUAL_LN.len--;
    V_file->ch_amount--;

    if(!memory__shrink_lines_array(V_file))
    {
        return false;
    }
    return true;
}

bool edit__delete_last_line(V_file_t* V_file)
{
    free(V_FILE__LAST_LN.txt);

    V_file->ln_amount--;
    return memory__shrink_lines_array(V_file);
}

void edit__filename(V_file_t* V_file, const Conf_t* const Config, Mod_t* Modes,
                    const char key)
{
    const char escape = ASCII__CTRL_LEFT_BRACKET;

    if((key >= 32) && (key != ASCII__BACKSPACE)
       && ((V_file->fname_len + SIZE__I) < PATH_MAX))
    {
        V_file->fname[V_file->fname_len] = key;
        V_file->fname[++V_file->fname_len] = '\0';
    }
    else if((key == ASCII__BACKSPACE) && (V_file->fname_len > 0))
    {
        V_file->fname[--V_file->fname_len] = '\0';
    }
    else if(key == '\n')
    {
        Modes->live_fname_edit = false;
        V_FILE__SET_STATUS("saved as with a different name");

        if(!strcmp(V_file->fname, V_file->fname_copy))
        {
            V_FILE__SET_STATUS("saved as");
        }
        file_io__save(V_file, Config);
        strncpy(V_file->fname_copy, V_file->fname, PATH_MAX);
    }
    else if(key == escape)
    {
        strncpy(V_file->fname, V_file->fname_copy, PATH_MAX);
        V_file->fname_len = strlen(V_file->fname);

        Modes->live_fname_edit = false;
        V_FILE__SET_STATUS("filename unchanged");
    }
}
