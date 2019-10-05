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
        if(V_FILE__CURSOR_X_SCROLLED)
        {
            if(!edit__move_lines_forward(v_file))
            {
                return false;
            }
        }
        else if(V_FILE__CURSOR_Y_SCROLLED)
        {
            if(!memory__copy_lines_forward(v_file))
            {
                return false;
            }
        }
        V_FILE__LAST_CHAR_IN_LINE = '\0';
    }
    return true;
}

bool keys__backspace(V_file* const v_file, const Config* const config,
                     const Modes* const modes)
{
    const size_t line_i_before_charange = V_FILE__ACTUAL_LINE_I;
    const char   tab_char             = (modes->tabs_to_spaces) ? ' ' : '\t';
    const size_t tab_sz             = (size_t) config->Tab_sz.value;

    for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
    {
        // Prevent removing a char and 3 tabs from that e.g.: "\t\t\t\t".
        if((V_FILE__CURSOR_X > 1)
           && (V_FILE__ACTUAL_LINE.txt[V_FILE__CURSOR_X - SIZE__NUL - SIZE__PREV]
               == tab_char)
           && (V_FILE__ACTUAL_LINE.txt[V_FILE__CURSOR_X - SIZE__NUL] != tab_char))
        {
            tab_i = (size_t) config->Tab_sz.value - SIZE__I;
        }

        /* Scenario when there is a char at the beginning and the Tab at the
           right. */
        if((V_FILE__CURSOR_X == 1) && (v_file->mirrored_cursor_x > 0)
           && (V_FILE__ACTUAL_LINE.txt[V_FILE__CURSOR_X - SIZE__NUL] != tab_char)
           && (V_FILE__ACTUAL_LINE.txt[V_FILE__CURSOR_X] == tab_char))
        {
            if(!edit__delete_char(v_file))
            {
                return false;
            }
            break;
        }
        if(!V_FILE__EMPTY_LINE)
        {
            if(!edit__delete_char(v_file))
            {
                return false;
            }
        }
        else if(!V_FILE__FIRST_LINE && !V_FILE__CURSOR_Y_SCROLLED
                && !edit__delete_last_empty_line(v_file))
        {
            return false;
        }

        // Some text and the Tab(s) at the end.
        if((V_FILE__ACTUAL_LINE.len > 0) && (v_file->mirrored_cursor_x == 0)
           && (V_FILE__ACTUAL_LINE.txt[V_FILE__ACTUAL_LINE.len - SIZE__NUL]
               != tab_char))
        {
            break;
        }
        /* Prevent removing a line when a first char in a line has to be
           removed. */
        else if((V_FILE__CURSOR_X == 0)
                && (V_FILE__ACTUAL_LINE.txt[V_FILE__CURSOR_X] != tab_char))
        {
            break;
        }
        // Scenario when there is the Tab and some text further.
        else if((V_FILE__CURSOR_X > 0) && (v_file->mirrored_cursor_x > 0)
                && (V_FILE__ACTUAL_LINE.txt[V_FILE__CURSOR_X - SIZE__NUL]
                    != tab_char))
        {
            break;
        }

        /* Prevents deleting [tab_width] lines at once with a maximally
           scrolled cursor in X. */
        if(line_i_before_charange != V_FILE__ACTUAL_LINE_I)
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

    if(!V_FILE__CURSOR_AT_LINE_START)
    {
        // Skip the e.g "\t\t\t\t" as the one Tab.
        for(size_t tab_i = 1; tab_i < tab_sz; tab_i++)
        {
            if(V_FILE__ACTUAL_LINE.txt[V_FILE__CURSOR_X - tab_i] != '\t')
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
    else if((v_file->lines_amount > 0) && !V_FILE__CURSOR_AT_TOP)
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

    if(V_FILE__CURSOR_X_SCROLLED)
    {
        // Skip the e.g "\t\t\t\t" as the one Tab.
        for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
        {
            if(V_FILE__ACTUAL_LINE.txt[V_FILE__CURSOR_X + tab_i] != '\t')
            {
                v_file->mirrored_cursor_x--;
                break; // No Tab, so don't skip anything.
            }
            else if(tab_i == (tab_sz - SIZE__I))
            {
                v_file->mirrored_cursor_x -= tab_sz;
            }
        }
        if(!V_FILE__CURSOR_X_SCROLLED && V_FILE__CURSOR_Y_SCROLLED)
        {
            v_file->mirrored_cursor_y--;
            v_file->mirrored_cursor_x = V_FILE__ACTUAL_LINE.len;
        }
        /* Last line doesn't contain the inefeed so ignoring it isn't
           necessary. */
        else if(!V_FILE__CURSOR_X_SCROLLED && (v_file->mirrored_cursor_y == 1))
        {
            v_file->mirrored_cursor_y--;
        }
    }
    v_file->esc_seq_on_input = false;
}

void keys__arrow_up(V_file* const v_file)
{
    if(!V_FILE__CURSOR_AT_TOP)
    {
        /* Cursor at a left side: doesn't go at a end of a line. Always at the
           beginning or ignore the linefeed. */
        v_file->mirrored_cursor_x = (V_FILE__CURSOR_AT_LINE_START)
                               ? V_FILE__PREV_LINE.len : SIZE__LF;
        v_file->mirrored_cursor_y++;
    }
    v_file->esc_seq_on_input = false;
}

void keys__arrow_down(V_file* const v_file)
{
    const bool cursor_at_prev_line_start = V_FILE__CURSOR_AT_LINE_START;

    if(V_FILE__CURSOR_Y_SCROLLED)
    {
        v_file->mirrored_cursor_y--;
        if(cursor_at_prev_line_start)
        {
            /* Cursor at the left side: doesn't go at a end of a line. Always
               at the beginning. */
            v_file->mirrored_cursor_x = V_FILE__ACTUAL_LINE.len;
        }
        else
        {
            // Ignore the LF or not.
            v_file->mirrored_cursor_x = (V_FILE__CURSOR_Y_SCROLLED) ? SIZE__LF : 0;
        }
    }
    v_file->esc_seq_on_input = false;
}

void keys__ctrl_arrow_left(V_file* const v_file)
{
    // Go to a previous line.
    if((V_FILE__CURSOR_X == 0) && (v_file->mirrored_cursor_y < v_file->lines_amount))
    {
        v_file->mirrored_cursor_y++;
        v_file->mirrored_cursor_x = SIZE__LF;
    }
    if((V_FILE__ACTUAL_CHAR != ' ') && (V_FILE__ACTUAL_CHAR != '\t'))
    {
        while((v_file->mirrored_cursor_x < V_FILE__ACTUAL_LINE.len)
              && !((V_FILE__ACTUAL_CHAR == ' ') || (V_FILE__ACTUAL_CHAR == '\t')))
        {
            v_file->mirrored_cursor_x++;
        }
        // Skip the Tab instantly instead of 1 column char for the first time.
        if(!V_FILE__EMPTY_LINE
           && ((V_FILE__ACTUAL_CHAR == ' ') || (V_FILE__ACTUAL_CHAR == '\t'))
           && ((V_FILE__ACTUAL_LINE.txt[V_FILE__CURSOR_X - SIZE__PREV] == ' ')
           || (V_FILE__ACTUAL_LINE.txt[V_FILE__CURSOR_X - SIZE__PREV] == '\t')))
        {
            // Prevents skipping only one part of the Tab.
            while((v_file->mirrored_cursor_x < V_FILE__ACTUAL_LINE.len)
                  && ((V_FILE__ACTUAL_CHAR == ' ')
                      || (V_FILE__ACTUAL_CHAR == '\t')))
            {
                v_file->mirrored_cursor_x++;
            }
            if((V_FILE__ACTUAL_CHAR != ' ') && (V_FILE__ACTUAL_CHAR != '\t'))
            {
                v_file->mirrored_cursor_x--; // Don't stop on a printable char.
            }
        }
    }
    else // Non-whitespace chars.
    {
        while((v_file->mirrored_cursor_x < V_FILE__ACTUAL_LINE.len)
              && ((V_FILE__ACTUAL_CHAR == ' ') || (V_FILE__ACTUAL_CHAR == '\t')))
        {
            v_file->mirrored_cursor_x++;
        }
        // Skip a whole word at once instead of 1 char for the first time.
        if(!((V_FILE__ACTUAL_CHAR == ' ') || (V_FILE__ACTUAL_CHAR == '\t')))
        {
            while((v_file->mirrored_cursor_x < V_FILE__ACTUAL_LINE.len)
                  && !((V_FILE__ACTUAL_CHAR == ' ')
                       || (V_FILE__ACTUAL_CHAR == '\t')))
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
    if((v_file->mirrored_cursor_x == 1) && V_FILE__CURSOR_Y_SCROLLED)
    {
        v_file->mirrored_cursor_y--;
        v_file->mirrored_cursor_x = V_FILE__ACTUAL_LINE.len;
    }
    if((V_FILE__ACTUAL_CHAR != ' ') && (V_FILE__ACTUAL_CHAR != '\t'))
    {
        while((v_file->mirrored_cursor_x > SIZE__NUL)
              && !((V_FILE__ACTUAL_CHAR == ' ') || (V_FILE__ACTUAL_CHAR == '\t')))
        {
            v_file->mirrored_cursor_x--;
        }
    }
    else // Non-whitespace chars.
    {
        while((v_file->mirrored_cursor_x > SIZE__NUL)
              && ((V_FILE__ACTUAL_CHAR == ' ') || (V_FILE__ACTUAL_CHAR == '\t')))
        {
            v_file->mirrored_cursor_x--;
        }
        // Don't stop before a last part of the Tab.
        if((V_FILE__ACTUAL_CHAR == ' ') || (V_FILE__ACTUAL_CHAR == '\t'))
        {
            v_file->mirrored_cursor_x--;
        }
    }
    v_file->esc_seq_on_input = false;
}

void keys__ctrl_arrow_up(V_file* const v_file)
{
    if(!V_FILE__CURSOR_AT_TOP)
    {
        for(;;)
        {
            v_file->mirrored_cursor_y++;
            if((V_FILE__ACTUAL_LINE.txt[0] == '\n') || V_FILE__CURSOR_AT_TOP)
            {
                break;
            }
        }
    }
    v_file->mirrored_cursor_x     = V_FILE__ACTUAL_LINE.len;
    v_file->esc_seq_on_input = false;
}

void keys__ctrl_arrow_down(V_file* const v_file)
{
    if(v_file->mirrored_cursor_y > 0) // Not at a bottom of the file.
    {
        for(;;)
        {
            v_file->mirrored_cursor_y--;
            if((V_FILE__ACTUAL_LINE.txt[0] == '\n')
               || (v_file->mirrored_cursor_y == 0))
            {
                break;
            }
        }
    }
    v_file->mirrored_cursor_x     = V_FILE__ACTUAL_LINE.len;
    v_file->esc_seq_on_input = false;
}
