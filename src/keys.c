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
        v_file_actual_line(v_file)->txt[
            v_file_actual_line(v_file)->length] = '\0';
    }
    return true;
}

bool keys__backspace(V_file* const v_file, const Config* const config,
                     const Modes* const modes)
{
    const size_t line_i_before_charange = v_file_cursor_y(v_file);
    const char   tab_char               = (modes->tabs_to_spaces) ? ' ' : '\t';
    const size_t tab_sz                 = (size_t) config->Tab_sz.value;

    for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
    {
        // Prevent removing a char and 3 tabs from that e.g.: "\t\t\t\t".
        if((v_file_cursor_x(v_file) > 1)
           && (v_file_actual_line(v_file)->txt[
                   v_file_cursor_x(v_file) - SIZE__NUL - SIZE__PREV]
               == tab_char)
           && (v_file_actual_line(v_file)->txt[
                   v_file_cursor_x(v_file) - SIZE__NUL] != tab_char))
        {
            tab_i = (size_t) config->Tab_sz.value - SIZE__I;
        }

        /* Scenario when there is a char at the beginning and the Tab at the
           right. */
        if((v_file_cursor_x(v_file) == 1) && (v_file->mirrored_cursor_x > 0)
           && (v_file_actual_line(v_file)->txt[
                   v_file_cursor_x(v_file) - SIZE__NUL] != tab_char)
           && (v_file_actual_line(v_file)->txt[
                   v_file_cursor_x(v_file)] == tab_char))
        {
            if(!edit__delete_char(v_file))
            {
                return false;
            }
            break;
        }
        if(!v_file__is_actual_line_empty(v_file))
        {
            if(!edit__delete_char(v_file))
            {
                return false;
            }
        }
        else if(!v_file__is_actual_line_first(v_file)
                && !v_file__is_cursor_y_scrolled(v_file)
                && !edit__delete_last_empty_line(v_file))
        {
            return false;
        }

        // Some txt and the Tab(s) at the end.
        if((v_file_actual_line(v_file)->length > 0)
           && (v_file->mirrored_cursor_x == 0)
           && (v_file_actual_line(v_file)->txt[
                   v_file_actual_line(v_file)->length - SIZE__NUL]
               != tab_char))
        {
            break;
        }
        /* Prevent removing a line when a first char in a line has to be
           removed. */
        else if((v_file_cursor_x(v_file) == 0)
                && (v_file_actual_line(v_file)->txt[
                        v_file_cursor_x(v_file)] != tab_char))
        {
            break;
        }
        // Scenario when there is the Tab and some txt further.
        else if((v_file_cursor_x(v_file) > 0)
                && (v_file->mirrored_cursor_x > 0)
                && (v_file_actual_line(v_file)->txt[
                        v_file_cursor_x(v_file) - SIZE__NUL]
                    != tab_char))
        {
            break;
        }

        /* Prevents deleting [tab_width] lines at once with a maximally
           scrolled cursor in X. */
        if(line_i_before_charange != v_file_cursor_y(v_file))
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

    const size_t tab_sz = (size_t) config->Tab_sz.value;
    const char   tab_char = (modes->tabs_to_spaces) ? ' ' : '\t';

    // Prevent the not-full Tab insert.
    if(v_file->chars_amount <= (size_t) (V_FILE__CHAR_MAX - tab_sz))
    {
        for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
        {
            if(!input__printable_char(v_file, tab_char))
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
    const size_t tab_sz = (size_t) config->Tab_sz.value;

    if(!v_file__is_cursor_at_line_start(v_file))
    {
        // Skip the e.g "\t\t\t\t" as the one Tab.
        for(size_t tab_i = 1; tab_i < tab_sz; tab_i++)
        {
            if(v_file_actual_line(v_file)->txt[v_file_cursor_x(v_file) - tab_i]
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
    const size_t tab_sz = (size_t) config->Tab_sz.value;

    if(v_file__is_cursor_x_scrolled(v_file))
    {
        // Skip the e.g "\t\t\t\t" as the one Tab.
        for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
        {
            if(v_file_actual_line(v_file)->txt[v_file_cursor_x(v_file) + tab_i]
               != '\t')
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
            v_file->mirrored_cursor_x = v_file_actual_line(v_file)->length;
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
                                    ? v_file_prev_line(v_file)->length
                                    : SIZE__LF;
        v_file->mirrored_cursor_y++;
    }
    v_file->esc_seq_on_input = false;
}

void keys__arrow_down(V_file* const v_file)
{
    const bool cursor_at_prev_line_start
        = v_file__is_cursor_at_line_start(v_file);

    if(v_file__is_cursor_y_scrolled(v_file))
    {
        v_file->mirrored_cursor_y--;
        if(cursor_at_prev_line_start)
        {
            /* Cursor at the left side: doesn't go at a end of a line. Always
               at the beginning. */
            v_file->mirrored_cursor_x = v_file_actual_line(v_file)->length;
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
    if((v_file_cursor_x(v_file) == 0)
       && (v_file->mirrored_cursor_y < v_file->lines_amount))
    {
        v_file->mirrored_cursor_y++;
        v_file->mirrored_cursor_x = SIZE__LF;
    }
    if((v_file_actual_char(v_file) != ' ')
       && (v_file_actual_char(v_file) != '\t'))
    {
        while((v_file->mirrored_cursor_x < v_file_actual_line(v_file)->length)
              && !((v_file_actual_char(v_file) == ' ')
                   || (v_file_actual_char(v_file) == '\t')))
        {
            v_file->mirrored_cursor_x++;
        }
        // Skip the Tab instantly instead of 1 column char for the first time.
        if(!v_file__is_actual_line_empty(v_file)
           && ((v_file_actual_char(v_file) == ' ')
               || (v_file_actual_char(v_file) == '\t'))
           && ((v_file_actual_line(v_file)->txt[
                    v_file_cursor_x(v_file) - SIZE__PREV] == ' ')
           || (v_file_actual_line(v_file)->txt[
                   v_file_cursor_x(v_file) - SIZE__PREV] == '\t')))
        {
            // Prevents skipping only one part of the Tab.
            while((v_file->mirrored_cursor_x
                   < v_file_actual_line(v_file)->length)
                  && ((v_file_actual_char(v_file) == ' ')
                      || (v_file_actual_char(v_file) == '\t')))
            {
                v_file->mirrored_cursor_x++;
            }
            if((v_file_actual_char(v_file) != ' ')
               && (v_file_actual_char(v_file) != '\t'))
            {
                v_file->mirrored_cursor_x--; // Don't stop on a printable char.
            }
        }
    }
    else // Non-whitespace chars.
    {
        while((v_file->mirrored_cursor_x < v_file_actual_line(v_file)->length)
              && ((v_file_actual_char(v_file) == ' ')
                  || (v_file_actual_char(v_file) == '\t')))
        {
            v_file->mirrored_cursor_x++;
        }
        // Skip a whole word at once instead of 1 char for the first time.
        if(!((v_file_actual_char(v_file) == ' ')
             || (v_file_actual_char(v_file) == '\t')))
        {
            while((v_file->mirrored_cursor_x
                   < v_file_actual_line(v_file)->length)
                  && !((v_file_actual_char(v_file) == ' ')
                       || (v_file_actual_char(v_file) == '\t')))
            {
                v_file->mirrored_cursor_x++;
            }
        }
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
        v_file->mirrored_cursor_x = v_file_actual_line(v_file)->length;
    }
    if((v_file_actual_char(v_file) != ' ')
       && (v_file_actual_char(v_file) != '\t'))
    {
        while((v_file->mirrored_cursor_x > SIZE__NUL)
              && !((v_file_actual_char(v_file) == ' ')
                   || (v_file_actual_char(v_file) == '\t')))
        {
            v_file->mirrored_cursor_x--;
        }
    }
    else // Non-whitespace chars.
    {
        while((v_file->mirrored_cursor_x > SIZE__NUL)
              && ((v_file_actual_char(v_file) == ' ')
                  || (v_file_actual_char(v_file) == '\t')))
        {
            v_file->mirrored_cursor_x--;
        }
        // Don't stop before a last part of the Tab.
        if((v_file_actual_char(v_file) == ' ')
           || (v_file_actual_char(v_file) == '\t'))
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
            if((v_file_actual_line(v_file)->txt[0] == '\n')
               || v_file__is_cursor_at_top(v_file))
            {
                break;
            }
        }
    }
    v_file->mirrored_cursor_x = v_file_actual_line(v_file)->length;
    v_file->esc_seq_on_input  = false;
}

void keys__ctrl_arrow_down(V_file* const v_file)
{
    if(v_file->mirrored_cursor_y > 0) // Not at a bottom of the file.
    {
        for(;;)
        {
            v_file->mirrored_cursor_y--;
            if((v_file_actual_line(v_file)->txt[0] == '\n')
               || (v_file->mirrored_cursor_y == 0))
            {
                break;
            }
        }
    }
    v_file->mirrored_cursor_x     = v_file_actual_line(v_file)->length;
    v_file->esc_seq_on_input = false;
}
