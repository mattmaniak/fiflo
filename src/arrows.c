#include "buffer.h"
#include "arrows.h"

void arrows__arrow_left(Buff_t* const Buffer, const Conf_t* const Config)
{
    const size_t tab_sz = (const size_t) Config->Tab_sz.value;

    if(!BUFFER__CURSOR_AT_LN_START)
    {
        // Skip the e.g "\t\t\t\t" as the one Tab.
        for(size_t tab_i = 1; tab_i < tab_sz; tab_i++)
        {
            if(BUFFER__ACTUAL_LN.txt[BUFFER__CURSOR_X - tab_i] != '\t')
            {
                Buffer->cursor_rev_x++;
                break; // No Tab, so don't skip anything.
            }
            else if(tab_i == (tab_sz - SIZE__I))
            {
                Buffer->cursor_rev_x += tab_sz;
            }
        }

    }
    else if((Buffer->ln_amount > 0) && !BUFFER__CURSOR_AT_TOP)
    {
        // Set to a right part of a line ignoring it's linefeed.
        Buffer->cursor_rev_x = SIZE__LF;
        Buffer->cursor_rev_y++;
    }
    Buffer->esc_seq_on_input = false;
}

void arrows__arrow_right(Buff_t* const Buffer, const Conf_t* const Config)
{
    const size_t tab_sz = (const size_t) Config->Tab_sz.value;

    if(BUFFER__CURSOR_X_SCROLLED)
    {
        // Skip the e.g "\t\t\t\t" as the one Tab.
        for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
        {
            if(BUFFER__ACTUAL_LN.txt[BUFFER__CURSOR_X + tab_i] != '\t')
            {
                Buffer->cursor_rev_x--;
                break; // No Tab, so don't skip anything.
            }
            else if(tab_i == (tab_sz - SIZE__I))
            {
                Buffer->cursor_rev_x -= tab_sz;
            }
        }
        if(!BUFFER__CURSOR_X_SCROLLED && BUFFER__CURSOR_Y_SCROLLED)
        {
            Buffer->cursor_rev_y--;
            Buffer->cursor_rev_x = BUFFER__ACTUAL_LN.len;
        }
        /* Last line doesn't contain the inefeed so ignoring it isn't
           necessary. */
        else if(!BUFFER__CURSOR_X_SCROLLED && (Buffer->cursor_rev_y == 1))
        {
            Buffer->cursor_rev_y--;
        }
    }
    Buffer->esc_seq_on_input = false;
}

void arrows__arrow_up(Buff_t* const Buffer)
{
    if(!BUFFER__CURSOR_AT_TOP)
    {
        /* Cursor at a left side: doesn't go at a end of a line. Always at the
           beginning or ignore the linefeed. */
        Buffer->cursor_rev_x = (BUFFER__CURSOR_AT_LN_START)
                               ? BUFFER__PREV_LN.len : SIZE__LF;
        Buffer->cursor_rev_y++;
    }
    Buffer->esc_seq_on_input = false;
}

void arrows__arrow_down(Buff_t* const Buffer)
{
    const bool cursor_at_prev_ln_start = BUFFER__CURSOR_AT_LN_START;

    if(BUFFER__CURSOR_Y_SCROLLED)
    {
        Buffer->cursor_rev_y--;
        if(cursor_at_prev_ln_start)
        {
            /* Cursor at the left side: doesn't go at a end of a line. Always
               at the beginning. */
            Buffer->cursor_rev_x = BUFFER__ACTUAL_LN.len;
        }
        else
        {
            // Ignore the LF or not.
            Buffer->cursor_rev_x = (BUFFER__CURSOR_Y_SCROLLED) ? SIZE__LF : 0;
        }
    }
    Buffer->esc_seq_on_input = false;
}

void arrows__ctrl_arrow_left(Buff_t* const Buffer)
{
    // Go to a previous line.
    if((BUFFER__CURSOR_X == 0)
       && (Buffer->cursor_rev_y < Buffer->ln_amount))
    {
        Buffer->cursor_rev_y++;
        Buffer->cursor_rev_x = SIZE__LF;
    }
    if((BUFFER__ACTUAL_CH != ' ') && (BUFFER__ACTUAL_CH != '\t'))
    {
        while((Buffer->cursor_rev_x < BUFFER__ACTUAL_LN.len)
              && !((BUFFER__ACTUAL_CH == ' ') || (BUFFER__ACTUAL_CH == '\t')))
        {
            Buffer->cursor_rev_x++;
        }
        // Skip the Tab instantly instead of 1 column char for the first time.
        if(!BUFFER__EMPTY_LN
           && ((BUFFER__ACTUAL_CH == ' ') || (BUFFER__ACTUAL_CH == '\t'))
           && ((BUFFER__ACTUAL_LN.txt[BUFFER__CURSOR_X - SIZE__PREV] == ' ')
           || (BUFFER__ACTUAL_LN.txt[BUFFER__CURSOR_X - SIZE__PREV] == '\t')))
        {
            // Prevents skipping only one part of the Tab.
            while((Buffer->cursor_rev_x < BUFFER__ACTUAL_LN.len)
                  && ((BUFFER__ACTUAL_CH == ' ')
                      || (BUFFER__ACTUAL_CH == '\t')))
            {
                Buffer->cursor_rev_x++;
            }
            if((BUFFER__ACTUAL_CH != ' ') && (BUFFER__ACTUAL_CH != '\t'))
            {
                Buffer->cursor_rev_x--; // Don't stop on a printable char.
            }
        }
    }
    else // Non-whitespace chars.
    {
        while((Buffer->cursor_rev_x < BUFFER__ACTUAL_LN.len)
              && ((BUFFER__ACTUAL_CH == ' ')
                  || (BUFFER__ACTUAL_CH == '\t')))
        {
            Buffer->cursor_rev_x++;
        }
        // Skip a whole word at once instead of 1 char for the first time.
        if(!((BUFFER__ACTUAL_CH == ' ') || (BUFFER__ACTUAL_CH == '\t')))
        {
            while((Buffer->cursor_rev_x < BUFFER__ACTUAL_LN.len)
                  && !((BUFFER__ACTUAL_CH == ' ')
                       || (BUFFER__ACTUAL_CH == '\t')))
            {
                Buffer->cursor_rev_x++;
            }
        }
    }
    Buffer->esc_seq_on_input = false;
}

void arrows__ctrl_arrow_right(Buff_t* const Buffer)
{
    // Go to a next line.
    if((Buffer->cursor_rev_x == 1) && BUFFER__CURSOR_Y_SCROLLED)
    {
        Buffer->cursor_rev_y--;
        Buffer->cursor_rev_x = BUFFER__ACTUAL_LN.len;
    }
    if((BUFFER__ACTUAL_CH != ' ') && (BUFFER__ACTUAL_CH != '\t'))
    {
        while((Buffer->cursor_rev_x > SIZE__NUL)
              && !((BUFFER__ACTUAL_CH == ' ')
                   || (BUFFER__ACTUAL_CH == '\t')))
        {
            Buffer->cursor_rev_x--;
        }
    }
    else // Non-whitespace chars.
    {
        while((Buffer->cursor_rev_x > SIZE__NUL)
              && ((BUFFER__ACTUAL_CH == ' ')
                  || (BUFFER__ACTUAL_CH == '\t')))
        {
            Buffer->cursor_rev_x--;
        }
        // Don't stop before a last part of the Tab.
        if((BUFFER__ACTUAL_CH == ' ') || (BUFFER__ACTUAL_CH == '\t'))
        {
            Buffer->cursor_rev_x--;
        }
    }
    Buffer->esc_seq_on_input = false;
}

void arrows__ctrl_arrow_up(Buff_t* const Buffer)
{
    if(!BUFFER__CURSOR_AT_TOP)
    {
        for(;;)
        {
            Buffer->cursor_rev_y++;
            if((BUFFER__ACTUAL_LN.txt[0] == '\n') || BUFFER__CURSOR_AT_TOP)
            {
                break;
            }
        }
    }
    Buffer->cursor_rev_x     = BUFFER__ACTUAL_LN.len;
    Buffer->esc_seq_on_input = false;
}

void arrows__ctrl_arrow_down(Buff_t* const Buffer)
{
    if(Buffer->cursor_rev_y > 0) // Not at a bottom of the file.
    {
        for(;;)
        {
            Buffer->cursor_rev_y--;

            if((BUFFER__ACTUAL_LN.txt[0] == '\n')
               || (Buffer->cursor_rev_y == 0))
            {
                break;
            }
        }
    }
    Buffer->cursor_rev_x     = BUFFER__ACTUAL_LN.len;
    Buffer->esc_seq_on_input = false;
}
