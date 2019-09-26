#include "arrows.h"

void arrows__arrow_left(V_file_t* const V_file, const Conf_t* const Config)
{
    const size_t tab_sz = (size_t) Config->Tab_sz.value;

    if(!V_FILE__CURSOR_AT_LN_START)
    {
        // Skip the e.g "\t\t\t\t" as the one Tab.
        for(size_t tab_i = 1; tab_i < tab_sz; tab_i++)
        {
            if(V_FILE__ACTUAL_LN.txt[V_FILE__CURSOR_X - tab_i] != '\t')
            {
                V_file->cursor_rev_x++;
                break; // No Tab, so don't skip anything.
            }
            else if(tab_i == (tab_sz - SIZE__I))
            {
                V_file->cursor_rev_x += tab_sz;
            }
        }

    }
    else if((V_file->ln_amount > 0) && !V_FILE__CURSOR_AT_TOP)
    {
        // Set to a right part of a line ignoring it's linefeed.
        V_file->cursor_rev_x = SIZE__LF;
        V_file->cursor_rev_y++;
    }
    V_file->esc_seq_on_input = false;
}

void arrows__arrow_right(V_file_t* const V_file, const Conf_t* const Config)
{
    const size_t tab_sz = (size_t) Config->Tab_sz.value;

    if(V_FILE__CURSOR_X_SCROLLED)
    {
        // Skip the e.g "\t\t\t\t" as the one Tab.
        for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
        {
            if(V_FILE__ACTUAL_LN.txt[V_FILE__CURSOR_X + tab_i] != '\t')
            {
                V_file->cursor_rev_x--;
                break; // No Tab, so don't skip anything.
            }
            else if(tab_i == (tab_sz - SIZE__I))
            {
                V_file->cursor_rev_x -= tab_sz;
            }
        }
        if(!V_FILE__CURSOR_X_SCROLLED && V_FILE__CURSOR_Y_SCROLLED)
        {
            V_file->cursor_rev_y--;
            V_file->cursor_rev_x = V_FILE__ACTUAL_LN.len;
        }
        /* Last line doesn't contain the inefeed so ignoring it isn't
           necessary. */
        else if(!V_FILE__CURSOR_X_SCROLLED && (V_file->cursor_rev_y == 1))
        {
            V_file->cursor_rev_y--;
        }
    }
    V_file->esc_seq_on_input = false;
}

void arrows__arrow_up(V_file_t* const V_file)
{
    if(!V_FILE__CURSOR_AT_TOP)
    {
        /* Cursor at a left side: doesn't go at a end of a line. Always at the
           beginning or ignore the linefeed. */
        V_file->cursor_rev_x = (V_FILE__CURSOR_AT_LN_START)
                               ? V_FILE__PREV_LN.len : SIZE__LF;
        V_file->cursor_rev_y++;
    }
    V_file->esc_seq_on_input = false;
}

void arrows__arrow_down(V_file_t* const V_file)
{
    const bool cursor_at_prev_ln_start = V_FILE__CURSOR_AT_LN_START;

    if(V_FILE__CURSOR_Y_SCROLLED)
    {
        V_file->cursor_rev_y--;
        if(cursor_at_prev_ln_start)
        {
            /* Cursor at the left side: doesn't go at a end of a line. Always
               at the beginning. */
            V_file->cursor_rev_x = V_FILE__ACTUAL_LN.len;
        }
        else
        {
            // Ignore the LF or not.
            V_file->cursor_rev_x = (V_FILE__CURSOR_Y_SCROLLED) ? SIZE__LF : 0;
        }
    }
    V_file->esc_seq_on_input = false;
}

void arrows__ctrl_arrow_left(V_file_t* const V_file)
{
    // Go to a previous line.
    if((V_FILE__CURSOR_X == 0) && (V_file->cursor_rev_y < V_file->ln_amount))
    {
        V_file->cursor_rev_y++;
        V_file->cursor_rev_x = SIZE__LF;
    }
    if((V_FILE__ACTUAL_CH != ' ') && (V_FILE__ACTUAL_CH != '\t'))
    {
        while((V_file->cursor_rev_x < V_FILE__ACTUAL_LN.len)
              && !((V_FILE__ACTUAL_CH == ' ') || (V_FILE__ACTUAL_CH == '\t')))
        {
            V_file->cursor_rev_x++;
        }
        // Skip the Tab instantly instead of 1 column char for the first time.
        if(!V_FILE__EMPTY_LN
           && ((V_FILE__ACTUAL_CH == ' ') || (V_FILE__ACTUAL_CH == '\t'))
           && ((V_FILE__ACTUAL_LN.txt[V_FILE__CURSOR_X - SIZE__PREV] == ' ')
           || (V_FILE__ACTUAL_LN.txt[V_FILE__CURSOR_X - SIZE__PREV] == '\t')))
        {
            // Prevents skipping only one part of the Tab.
            while((V_file->cursor_rev_x < V_FILE__ACTUAL_LN.len)
                  && ((V_FILE__ACTUAL_CH == ' ')
                      || (V_FILE__ACTUAL_CH == '\t')))
            {
                V_file->cursor_rev_x++;
            }
            if((V_FILE__ACTUAL_CH != ' ') && (V_FILE__ACTUAL_CH != '\t'))
            {
                V_file->cursor_rev_x--; // Don't stop on a printable char.
            }
        }
    }
    else // Non-whitespace chars.
    {
        while((V_file->cursor_rev_x < V_FILE__ACTUAL_LN.len)
              && ((V_FILE__ACTUAL_CH == ' ') || (V_FILE__ACTUAL_CH == '\t')))
        {
            V_file->cursor_rev_x++;
        }
        // Skip a whole word at once instead of 1 char for the first time.
        if(!((V_FILE__ACTUAL_CH == ' ') || (V_FILE__ACTUAL_CH == '\t')))
        {
            while((V_file->cursor_rev_x < V_FILE__ACTUAL_LN.len)
                  && !((V_FILE__ACTUAL_CH == ' ')
                       || (V_FILE__ACTUAL_CH == '\t')))
            {
                V_file->cursor_rev_x++;
            }
        }
    }
    V_file->esc_seq_on_input = false;
}

void arrows__ctrl_arrow_right(V_file_t* const V_file)
{
    // Go to a next line.
    if((V_file->cursor_rev_x == 1) && V_FILE__CURSOR_Y_SCROLLED)
    {
        V_file->cursor_rev_y--;
        V_file->cursor_rev_x = V_FILE__ACTUAL_LN.len;
    }
    if((V_FILE__ACTUAL_CH != ' ') && (V_FILE__ACTUAL_CH != '\t'))
    {
        while((V_file->cursor_rev_x > SIZE__NUL)
              && !((V_FILE__ACTUAL_CH == ' ') || (V_FILE__ACTUAL_CH == '\t')))
        {
            V_file->cursor_rev_x--;
        }
    }
    else // Non-whitespace chars.
    {
        while((V_file->cursor_rev_x > SIZE__NUL)
              && ((V_FILE__ACTUAL_CH == ' ') || (V_FILE__ACTUAL_CH == '\t')))
        {
            V_file->cursor_rev_x--;
        }
        // Don't stop before a last part of the Tab.
        if((V_FILE__ACTUAL_CH == ' ') || (V_FILE__ACTUAL_CH == '\t'))
        {
            V_file->cursor_rev_x--;
        }
    }
    V_file->esc_seq_on_input = false;
}

void arrows__ctrl_arrow_up(V_file_t* const V_file)
{
    if(!V_FILE__CURSOR_AT_TOP)
    {
        for(;;)
        {
            V_file->cursor_rev_y++;
            if((V_FILE__ACTUAL_LN.txt[0] == '\n') || V_FILE__CURSOR_AT_TOP)
            {
                break;
            }
        }
    }
    V_file->cursor_rev_x     = V_FILE__ACTUAL_LN.len;
    V_file->esc_seq_on_input = false;
}

void arrows__ctrl_arrow_down(V_file_t* const V_file)
{
    if(V_file->cursor_rev_y > 0) // Not at a bottom of the file.
    {
        for(;;)
        {
            V_file->cursor_rev_y--;
            if((V_FILE__ACTUAL_LN.txt[0] == '\n')
               || (V_file->cursor_rev_y == 0))
            {
                break;
            }
        }
    }
    V_file->cursor_rev_x     = V_FILE__ACTUAL_LN.len;
    V_file->esc_seq_on_input = false;
}
