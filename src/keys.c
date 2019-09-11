#include "buffer.h"
#include "keys.h"

void keys__arrow_left(Buff_t* Buffer, const Conf_t* const Config)
{
    const idx_t tab_sz = (const idx_t) Config->Tab_sz.value;

    if(!BUFFER__CURSOR_AT_LINE_BEGINNING)
    {
        // Skip the e.g "\t\t\t\t" as the one Tab.
        for(idx_t tab_idx = 1; tab_idx < tab_sz; tab_idx++)
        {
            if(BUFFER__ACTUAL_LINE[BUFFER__CURSOR_X - tab_idx] != '\t')
            {
                Buffer->cursor_rev_x++;
                break; // No Tab, so don't skip anything.
            }
            else if(tab_idx == (idx_t) Config->Tab_sz.value - IDX)
            {
                Buffer->cursor_rev_x += (idx_t) Config->Tab_sz.value;
            }
        }

    }
    else if((Buffer->lines_amount > 0) && !BUFFER__CURSOR_AT_TOP)
    {
        // Set to a right part of a line ignoring it's linefeed.
        Buffer->cursor_rev_x = LF_SZ;
        Buffer->cursor_rev_y++;
    }
    Buffer->escape_sequence_on_input = false;
}

void keys__arrow_right(Buff_t* Buffer, const Conf_t* const Config)
{
    const idx_t tab_sz = (const idx_t) Config->Tab_sz.value;

    if(BUFFER__CURSOR_X_SCROLLED)
    {
        // Skip the e.g "\t\t\t\t" as the one Tab.
        for(idx_t tab_idx = 0; tab_idx < tab_sz; tab_idx++)
        {
            if(BUFFER__ACTUAL_LINE[BUFFER__CURSOR_X + tab_idx] != '\t')
            {
                Buffer->cursor_rev_x--;
                break; // No Tab, so don't skip anything.
            }
            else if(tab_idx == (idx_t) Config->Tab_sz.value - IDX)
            {
                Buffer->cursor_rev_x -= (idx_t) Config->Tab_sz.value;
            }
        }
        if(!BUFFER__CURSOR_X_SCROLLED && BUFFER__CURSOR_Y_SCROLLED)
        {
            Buffer->cursor_rev_y--;
            Buffer->cursor_rev_x = BUFFER__ACTUAL_LINE_LEN;
        }
        /* Last line doesn't contain the inefeed so ignoring it isn't
           necessary. */
        else if(!BUFFER__CURSOR_X_SCROLLED && (Buffer->cursor_rev_y == 1))
        {
            Buffer->cursor_rev_y--;
        }
    }
    Buffer->escape_sequence_on_input = false;
}

void keys__arrow_up(Buff_t* Buffer)
{
    if(!BUFFER__CURSOR_AT_TOP)
    {
        /* Cursor at a left side: doesn't go at a end of a line. Always at the
           beginning or ignore the linefeed. */
        Buffer->cursor_rev_x = (BUFFER__CURSOR_AT_LINE_BEGINNING)
                               ? BUFFER__PREV_LINE_LEN : LF_SZ;
        Buffer->cursor_rev_y++;
    }
    Buffer->escape_sequence_on_input = false;
}

void keys__arrow_down(Buff_t* Buffer)
{
    bool cursor_at_previous_line_start = BUFFER__CURSOR_AT_LINE_BEGINNING;

    if(BUFFER__CURSOR_Y_SCROLLED)
    {
        Buffer->cursor_rev_y--;
        if(cursor_at_previous_line_start)
        {
            /* Cursor at the left side: doesn't go at a end of a line. Always
               at the beginning. */
            Buffer->cursor_rev_x = BUFFER__ACTUAL_LINE_LEN;
        }
        else
        {
            // Ignore the LF or not.
            Buffer->cursor_rev_x = (BUFFER__CURSOR_Y_SCROLLED) ? LF_SZ : 0;
        }
    }
    Buffer->escape_sequence_on_input = false;
}

void keys__ctrl_arrow_left(Buff_t* Buffer)
{
    // Go to a previous line.
    if((BUFFER__CURSOR_X == 0)
       && (Buffer->cursor_rev_y < Buffer->lines_amount))
    {
        Buffer->cursor_rev_y++;
        Buffer->cursor_rev_x = LF_SZ;
    }
    if((BUFFER__ACTUAL_CHAR != ' ') && (BUFFER__ACTUAL_CHAR != '\t'))
    {
        while((Buffer->cursor_rev_x < BUFFER__ACTUAL_LINE_LEN)
              && !((BUFFER__ACTUAL_CHAR == ' ')
              || (BUFFER__ACTUAL_CHAR == '\t')))
        {
            Buffer->cursor_rev_x++;
        }
        // Skip the Tab instantly instead of 1 column char for the first time.
        if(!BUFFER__EMPTY_LINE && ((BUFFER__ACTUAL_CHAR == ' ')
           || (BUFFER__ACTUAL_CHAR == '\t'))
           && ((BUFFER__ACTUAL_LINE[BUFFER__CURSOR_X - PREV] == ' ')
           || (BUFFER__ACTUAL_LINE[BUFFER__CURSOR_X - PREV] == '\t')))
        {
            // Prevents skipping only one part of the Tab.
            while((Buffer->cursor_rev_x < BUFFER__ACTUAL_LINE_LEN)
                  && ((BUFFER__ACTUAL_CHAR == ' ')
                  || (BUFFER__ACTUAL_CHAR == '\t')))
            {
                Buffer->cursor_rev_x++;
            }
            if((BUFFER__ACTUAL_CHAR != ' ') && (BUFFER__ACTUAL_CHAR != '\t'))
            {
                Buffer->cursor_rev_x--; // Don't stop on a printable char.
            }
        }
    }
    else // Non-whitespace chars.
    {
        while((Buffer->cursor_rev_x < BUFFER__ACTUAL_LINE_LEN)
              && ((BUFFER__ACTUAL_CHAR == ' ')
              || (BUFFER__ACTUAL_CHAR == '\t')))
        {
            Buffer->cursor_rev_x++;
        }
        // Skip a whole word at once instead of 1 char for the first time.
        if(!((BUFFER__ACTUAL_CHAR == ' ') || (BUFFER__ACTUAL_CHAR == '\t')))
        {
            while((Buffer->cursor_rev_x < BUFFER__ACTUAL_LINE_LEN)
                  && !((BUFFER__ACTUAL_CHAR == ' ')
                  || (BUFFER__ACTUAL_CHAR == '\t')))
            {
                Buffer->cursor_rev_x++;
            }
        }
    }
    Buffer->escape_sequence_on_input = false;
}

void keys__ctrl_arrow_right(Buff_t* Buffer)
{
    // Go to a next line.
    if((Buffer->cursor_rev_x == 1) && BUFFER__CURSOR_Y_SCROLLED)
    {
        Buffer->cursor_rev_y--;
        Buffer->cursor_rev_x = BUFFER__ACTUAL_LINE_LEN;
    }
    if((BUFFER__ACTUAL_CHAR != ' ') && (BUFFER__ACTUAL_CHAR != '\t'))
    {
        while((Buffer->cursor_rev_x > NUL_SZ)
              && !((BUFFER__ACTUAL_CHAR == ' ')
              || (BUFFER__ACTUAL_CHAR == '\t')))
        {
            Buffer->cursor_rev_x--;
        }
    }
    else // Non-whitespace chars.
    {
        while((Buffer->cursor_rev_x > NUL_SZ) && ((BUFFER__ACTUAL_CHAR == ' ')
              || (BUFFER__ACTUAL_CHAR == '\t')))
        {
            Buffer->cursor_rev_x--;
        }
        // Don't stop before a last part of the Tab.
        if((BUFFER__ACTUAL_CHAR == ' ') || (BUFFER__ACTUAL_CHAR == '\t'))
        {
            Buffer->cursor_rev_x--;
        }
    }
    Buffer->escape_sequence_on_input = false;
}

void keys__ctrl_arrow_up(Buff_t* Buffer)
{
    if(!BUFFER__CURSOR_AT_TOP)
    {
        for(;;)
        {
            Buffer->cursor_rev_y++;

            if((BUFFER__ACTUAL_LINE[0] == '\n') || BUFFER__CURSOR_AT_TOP)
            {
                break;
            }
        }
    }
    Buffer->cursor_rev_x = BUFFER__ACTUAL_LINE_LEN;

    Buffer->escape_sequence_on_input = false;
}

void keys__ctrl_arrow_down(Buff_t* Buffer)
{
    if(Buffer->cursor_rev_y > 0) // Not at a bottom of the file.
    {
        for(;;)
        {
            Buffer->cursor_rev_y--;

            if((BUFFER__ACTUAL_LINE[0] == '\n')
               || (Buffer->cursor_rev_y == 0))
            {
                break;
            }
        }
    }
    Buffer->cursor_rev_x = BUFFER__ACTUAL_LINE_LEN;

    Buffer->escape_sequence_on_input = false;
}
