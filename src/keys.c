#include "keys.h"

bool keys__linefeed(V_file* const v_file)
{
    if(v_file->lines_amount < V_FILE__CHAR_MAX)
    {
        v_file->lines_amount++;
        if(!memory__extend_lines_array(v_file))
        {
            return false;
        }
        if(v_file__is_cursor_x_scrolled(v_file))
        {
            if(!edit__move_lines_forward(v_file))
            {
                return false;
            }
        }
        else if(v_file__is_cursor_y_scrolled(v_file))
        {
            if(!memory__copy_lines_forward(v_file))
            {
                return false;
            }
        }
        *v_file__last_char_in_actual_line(v_file) = '\0';
    }
    return true;
}

bool keys__backspace(V_file* const v_file, const Config* const config,
                     const Modes* const modes)
{
    const size_t ln_i_before_charange = v_file__cursor_y(v_file);
    const char   tab_ch               = (modes->tabs_to_spaces) ? ' ' : '\t';
    const size_t tab_sz               = (size_t) config->tab_sz.value;
    size_t       actual_char_x;

    for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
    {
        actual_char_x = v_file__cursor_x(v_file) - SIZE__I;

        tab_i = edit__dont_delete_char_after_tab(v_file, tab_sz, tab_ch,
                                                 tab_i);

        if((v_file__cursor_x(v_file) == 1) && (v_file->mirrored_cursor_x > 0)
           && (v_file__actual_line(v_file)->txt[actual_char_x] != tab_ch)
           && (*v_file__actual_char(v_file) == tab_ch))
        {
            if(!edit__delete_char(v_file))
            {
                return false;
            }
            break;
        }
        if(!edit__shift_tab_from_right(v_file))
        {
            return false;
        }
        actual_char_x = v_file__cursor_x(v_file) - SIZE__I;

        if(!edit__delete_char_before_tab(v_file, tab_ch, actual_char_x))
        {
            break;
        }

        /* Prevents deleting [tab_width] lines at once with a maximally
           scrolled cursor in X. */
        if(ln_i_before_charange != v_file__cursor_y(v_file))
        {
            break;
        }
    }
    V_FILE__SET_STATUS("edited");

    return true;
}

bool keys__tab(V_file* v_file, const Config* const config,
               const Modes* const modes)
{
    /* When the Tab key is pressed, it will insert e.g. 4 '\t' into a buffer.
       They will be converted during a rendering, loading and saving a file. */

    const size_t tab_sz = (size_t) config->tab_sz.value;
    const char   tab_ch = (modes->tabs_to_spaces) ? ' ' : '\t';

    // Prevent the not-full Tab insert.
    if(v_file->chars_amount <= (size_t) (V_FILE__CHAR_MAX - tab_sz))
    {
        for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
        {
            if(!input__printable_char(v_file, tab_ch))
            {
                return false;
            }
        }
    }
    else
    {
        V_FILE__SET_STATUS("can't read or insert more tabs");
    }
    return true;
}

void keys__arrow_left(V_file* const v_file, const Config* const config)
{
    const size_t tab_sz = (size_t) config->tab_sz.value;

    if(!v_file__is_cursor_at_line_start(v_file))
    {
        // Skip the e.g "\t\t\t\t" as the one Tab.
        for(size_t tab_i = 1; tab_i < tab_sz; tab_i++)
        {
            if(v_file__actual_line(v_file)->txt[v_file__cursor_x(v_file) - tab_i]
               != '\t')
            {
                v_file->mirrored_cursor_x++;
                break; // No Tab, so don't skip anything.
            }
            else if(tab_i == (tab_sz - SIZE__I))
            {
                v_file->mirrored_cursor_x += tab_sz;
            }
        }
    }
    else if((v_file->lines_amount > 0) && !v_file__is_cursor_at_top(v_file))
    {
        // Set to a right part of a line ignoring it's linefeed.
        v_file->mirrored_cursor_x = SIZE__LF;
        v_file->mirrored_cursor_y++;
    }
    v_file->esc_seq_on_input = false;
}

void keys__arrow_right(V_file* const v_file, const Config* const config)
{
    const size_t tab_sz = (size_t) config->tab_sz.value;
    size_t       ch_i;

    if(v_file__is_cursor_x_scrolled(v_file))
    {
        // Skip the e.g "\t\t\t\t" as the one Tab.
        for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
        {
            ch_i = v_file__cursor_x(v_file) + tab_i;

            if(v_file__actual_line(v_file)->txt[ch_i] != '\t')
            {
                v_file->mirrored_cursor_x--;
                break; // No Tab, so don't skip anything.
            }
            else if(tab_i == (tab_sz - SIZE__I))
            {
                v_file->mirrored_cursor_x -= tab_sz;
            }
        }
        if(!v_file__is_cursor_x_scrolled(v_file)
            && v_file__is_cursor_y_scrolled(v_file))
        {
            v_file->mirrored_cursor_y--;
            v_file->mirrored_cursor_x = v_file__actual_line(v_file)->len;
        }
        /* Last line doesn't contain the inefeed so ignoring it isn't
           necessary. */
        else if(!v_file__is_cursor_x_scrolled(v_file)
                && (v_file->mirrored_cursor_y == 1))
        {
            v_file->mirrored_cursor_y--;
        }
    }
    v_file->esc_seq_on_input = false;
}

void keys__arrow_up(V_file* const v_file)
{
    if(!v_file__is_cursor_at_top(v_file))
    {
        /* Cursor at a left side: doesn't go at a end of a line. Always at the
           beginning or ignore the linefeed. */
        v_file->mirrored_cursor_x = (v_file__is_cursor_at_line_start(v_file))
                                    ? v_file__prev_line(v_file)->len
                                    : SIZE__LF;
        v_file->mirrored_cursor_y++;
    }
    v_file->esc_seq_on_input = false;
}

void keys__arrow_down(V_file* const v_file)
{
    bool cursor_at_prev_line_start;

    cursor_at_prev_line_start = v_file__is_cursor_at_line_start(v_file);

    if(v_file__is_cursor_y_scrolled(v_file))
    {
        v_file->mirrored_cursor_y--;
        if(cursor_at_prev_line_start)
        {
            /* Cursor at the left side: doesn't go at a end of a line. Always
               at the beginning. */
            v_file->mirrored_cursor_x = v_file__actual_line(v_file)->len;
        }
        else
        {
            // Ignore the LF or not.
            v_file->mirrored_cursor_x = (v_file__is_cursor_y_scrolled(v_file))
                                        ? SIZE__LF : 0;
        }
    }
    v_file->esc_seq_on_input = false;
}

void keys__ctrl_arrow_left(V_file* const v_file)
{
    // Go to a previous line.
    if((v_file__cursor_x(v_file) == 0)
       && (v_file->mirrored_cursor_y < v_file->lines_amount))
    {
        v_file->mirrored_cursor_y++;
        v_file->mirrored_cursor_x = SIZE__LF;
    }
    if((*v_file__actual_char(v_file) != ' ')
       && (*v_file__actual_char(v_file) != '\t'))
    {
        while((v_file->mirrored_cursor_x < v_file__actual_line(v_file)->len)
              && !((*v_file__actual_char(v_file) == ' ')
                   || (*v_file__actual_char(v_file) == '\t')))
        {
            v_file->mirrored_cursor_x++;
        }
        edit__skip_tab_left(v_file);
    }
    else // Non-whitespace chars.
    {
        edit__skip_visible_chars_left(v_file);
    }
    v_file->esc_seq_on_input = false;
}

void keys__ctrl_arrow_right(V_file* const v_file)
{
    // Go to a next line.
    if((v_file->mirrored_cursor_x == 1)
       && v_file__is_cursor_y_scrolled(v_file))
    {
        v_file->mirrored_cursor_y--;
        v_file->mirrored_cursor_x = v_file__actual_line(v_file)->len;
    }
    if((*v_file__actual_char(v_file) != ' ')
       && (*v_file__actual_char(v_file) != '\t'))
    {
        while((v_file->mirrored_cursor_x > SIZE__I)
              && !((*v_file__actual_char(v_file) == ' ')
                   || (*v_file__actual_char(v_file) == '\t')))
        {
            v_file->mirrored_cursor_x--;
        }
    }
    else // Non-whitespace chars.
    {
        while((v_file->mirrored_cursor_x > SIZE__I)
              && ((*v_file__actual_char(v_file) == ' ')
                  || (*v_file__actual_char(v_file) == '\t')))
        {
            v_file->mirrored_cursor_x--;
        }
        // Don't stop before a last part of the Tab.
        if((*v_file__actual_char(v_file) == ' ')
           || (*v_file__actual_char(v_file) == '\t'))
        {
            v_file->mirrored_cursor_x--;
        }
    }
    v_file->esc_seq_on_input = false;
}

void keys__ctrl_arrow_up(V_file* const v_file)
{
    if(!v_file__is_cursor_at_top(v_file))
    {
        for(;;)
        {
            v_file->mirrored_cursor_y++;
            if((v_file__actual_line(v_file)->txt[0] == '\n')
               || v_file__is_cursor_at_top(v_file))
            {
                break;
            }
        }
    }
    v_file->mirrored_cursor_x = v_file__actual_line(v_file)->len;
    v_file->esc_seq_on_input  = false;
}

void keys__ctrl_arrow_down(V_file* const v_file)
{
    if(v_file->mirrored_cursor_y > 0) // Not at a bottom of the file.
    {
        for(;;)
        {
            v_file->mirrored_cursor_y--;
            if((v_file__actual_line(v_file)->txt[0] == '\n')
               || (v_file->mirrored_cursor_y == 0))
            {
                break;
            }
        }
    }
    v_file->mirrored_cursor_x = v_file__actual_line(v_file)->len;
    v_file->esc_seq_on_input  = false;
}
