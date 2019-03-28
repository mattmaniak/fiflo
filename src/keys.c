#include "buffer.h"
#include "keys.h"

void keys__arrow_left(Buff_t* Buffer, Conf_t* Config, Mod_t* Modes)
{
    bool more_than_one_line = Buffer->lines_idx > 0;

    if(Modes->live_fname_edit)
    {
        if(Buffer->fname_cursor_rev_x < strlen(Buffer->fname))
        {
            Buffer->fname_cursor_rev_x++; // Scroll left.
        }
    }

    if(!CURSOR_AT_LINE_START)
    {
        // Skip the e.g "\t\t\t\t" as the one tab.
        for(idx_t tab_idx = 1; tab_idx < (idx_t) Config->Tab_width.value;
            tab_idx++)
        {
            if(CURRENT_LINE[CURSOR_X_IDX - tab_idx] != '\t')
            {
                Buffer->cursor_rev_x++;
                break; // No tab, so don't skip anything.
            }
            else if(tab_idx == (idx_t) Config->Tab_width.value - IDX)
            {
                Buffer->cursor_rev_x += (idx_t) Config->Tab_width.value;
            }
        }

    }
    else if(more_than_one_line && !CURSOR_AT_TOP)
    {
        // Set to the right ignoring the keys__linefeed.
        Buffer->cursor_rev_x = 1;
        Buffer->cursor_rev_y++;
    }
    Buffer->escape_sequence_on_input = false;
}

void keys__arrow_right(Buff_t* Buffer, Conf_t* Config, Mod_t* Modes)
{
    if(Modes->live_fname_edit)
    {
        if(Buffer->fname_cursor_rev_x > 0)
        {
            Buffer->fname_cursor_rev_x--; // Scroll right.
        }
    }

    if(CURSOR_X_SCROLLED)
    {
        // Skip the e.g "\t\t\t\t" as the one tab.
        for(idx_t tab_idx = 0; tab_idx < (idx_t) Config->Tab_width.value;
            tab_idx++)
        {
            if(CURRENT_LINE[CURSOR_X_IDX + tab_idx] != '\t')
            {
                Buffer->cursor_rev_x--;
                break; // No tab, so don't skip anything.
            }
            else if(tab_idx == (idx_t) Config->Tab_width.value - IDX)
            {
                Buffer->cursor_rev_x -= (idx_t) Config->Tab_width.value;
            }
        }
        if(!CURSOR_X_SCROLLED && CURSOR_Y_SCROLLED)
        {
            Buffer->cursor_rev_y--;
            Buffer->cursor_rev_x = CURRENT_LINE_LENGTH_IDX;
        }
        /* Last line doesn't contain keys__linefeed so ignoring that isn't
        necessary. */
        else if(!CURSOR_X_SCROLLED && (Buffer->cursor_rev_y == 1))
        {
            Buffer->cursor_rev_y--;
        }
    }
    Buffer->escape_sequence_on_input = false;
}

void keys__arrow_up(Buff_t* Buffer)
{
    if(!CURSOR_AT_TOP)
    {
        /* Cursor at the left side: doesn't go at the end of a line. Always
        at the beginning or ignore the linefeed. */
        Buffer->cursor_rev_x = (CURSOR_AT_LINE_START)
                               ? PREVIOUS_LINE_LENGTH_IDX : LF_SZ;
        Buffer->cursor_rev_y++;
    }
    Buffer->escape_sequence_on_input = false;
}

void keys__arrow_down(Buff_t* Buffer)
{
    bool cursor_at_previous_line_start = CURSOR_AT_LINE_START;

    if(CURSOR_Y_SCROLLED)
    {
        Buffer->cursor_rev_y--;
        if(cursor_at_previous_line_start)
        {
            /* Cursor at the left side: doesn't go at the end of a line. Always
            at the beginning. */
            Buffer->cursor_rev_x = CURRENT_LINE_LENGTH_IDX;
        }
        else
        {
            // Ignore the LF or not.
            Buffer->cursor_rev_x = (CURSOR_Y_SCROLLED) ? LF_SZ : 0;
        }
    }
    Buffer->escape_sequence_on_input = false;
}

void keys__ctrl__arrow_up(Buff_t* Buffer)
{
    Buffer->cursor_rev_y = Buffer->lines_idx;
    Buffer->cursor_rev_x = CURRENT_LINE_LENGTH_IDX;

    Buffer->escape_sequence_on_input = false;
}

void keys__ctrl__arrow_down(Buff_t* Buffer)
{
    Buffer->cursor_rev_y = 0;
    Buffer->cursor_rev_x = CURRENT_LINE_LENGTH_IDX;

    Buffer->escape_sequence_on_input = false;
}
