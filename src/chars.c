#include "chars.h"

bool chars__parse_char(Buff_t* Buffer, const Conf_t* const Config, Mod_t* Modes,
                       const char ch)
{
    switch(ch)
    {
    default:
        return chars__printable_char(Buffer, ch);

    case '\t':
        return chars__tab(Buffer, Config);

    case BACKSPACE:
        return chars__backspace(Buffer, Config);

    case CTRL_Q:
        return false;

    case CTRL_S:
        return file__save(Buffer, Config);

    case CTRL_BACKSLASH:
        Modes->lbar_toggled = !Modes->lbar_toggled;
        break;

    case CTRL_D:
        return edit__delete_line(Buffer);

    case CTRL_O:
        Modes->live_fname_edit = true;
    }
    return true;
}

bool chars__linefeed(Buff_t* Buffer)
{
    if(Buffer->lines_idx < CHARS_MAX)
    {
        Buffer->lines_idx++;
        if(!memory__extend_lines_array(Buffer))
        {
            return false;
        }

        if(CURSOR_X_SCROLLED)
        {
            if(!edit__move_lines_forward(Buffer))
            {
                return false;
            }
        }
        else if(CURSOR_Y_SCROLLED)
        {
            if(!memory__copy_lines_forward(Buffer))
            {
                return false;
            }
        }
        LAST_CHAR_IN_LINE = '\0';
    }
    return true;
}

bool chars__printable_char(Buff_t* Buffer, const char ch)
{

#ifdef DEBUG_KEYS
    const bool in_charmap = true;
#else
    const bool in_charmap = (ch == '\0') || (ch == '\t') || (ch == '\n')
                            || (ch >= 32);
#endif

    if(in_charmap)
    {
        if(BUFFER_NOT_FULL)
        {
            Buffer->chars_idx++;
            CURRENT_LINE_LENGTH++;

            if(!memory__extend_line(Buffer, CURRENT_LINE_IDX))
            {
                return false;
            }

            if(CURSOR_X_SCROLLED)
            {
                edit__shift_text_horizonally(Buffer, 'r');
            }
            CURRENT_LINE[CURSOR_X - NUL_SZ] = ch;
            LAST_CHAR_IN_LINE               = '\0';

            // Initializing nul handler.
            if((ch == '\0') && !EMPTY_LINE)
            {
                Buffer->chars_idx--;
                CURRENT_LINE_LENGTH--;
            }
            else if(ch == '\n')
            {
                if(!chars__linefeed(Buffer))
                {
                    return false;
                }
            }
            if(ch != '\0')
            {
                SET_STATUS("edited");
            }
        }
        else
        {
            SET_STATUS("can't read or insert more chars");
        }
    }
    else
    {
        SET_STATUS("unsupported char(s)");
    }
    return true;
}

bool chars__backspace(Buff_t* Buffer, const Conf_t* const Config)
{
    const idx_t prev = 1;
    idx_t       remembered_line_idx = CURRENT_LINE_IDX;

    for(idx_t tab_idx = 0; tab_idx < (idx_t) Config->Tab_sz.value; tab_idx++)
    {
        // Prevent removind char and 3 tabs from that e.g.: "\t\t\t\t".
        if((CURSOR_X > 1) && (CURRENT_LINE[CURSOR_X - NUL_SZ - prev] == '\t')
           && (CURRENT_LINE[CURSOR_X - NUL_SZ] != '\t'))
        {
            tab_idx = (idx_t) Config->Tab_sz.value - IDX;
        }

        // Scenario when there is char at the beginning and tab at the right.
        if((CURSOR_X == 1) && (CURRENT_LINE[CURSOR_X - NUL_SZ] != '\t')
           && (CURRENT_LINE[CURSOR_X] == '\t') && (Buffer->cursor_rev_x > 0))
        {
            if(!edit__delete_char(Buffer))
            {
                return false;
            }
            break;
        }

        if(!EMPTY_LINE)
        {
            if(!edit__delete_char(Buffer))
            {
                return false;
            }
        }
        else if(!FIRST_LINE && !CURSOR_Y_SCROLLED)
        {
            if(!edit__delete_last_empty_line(Buffer))
            {
                return false;
            }
        }

        // Some text and the tab(s) at the end.
        if((CURRENT_LINE_LENGTH > 0) && (Buffer->cursor_rev_x == 0)
           && (CURRENT_LINE[CURRENT_LINE_LENGTH - NUL_SZ] != '\t'))
        {
            break;
        }
        /* Prevent removing the line when the first char in the line has to be
        removed. */
        else if((CURSOR_X == 0) && (CURRENT_LINE[CURSOR_X] != '\t'))
        {
            break;
        }
        // Scenario when there is the tab and some text further.
        else if((CURSOR_X > 0) && (Buffer->cursor_rev_x > 0)
                && (CURRENT_LINE[CURSOR_X - NUL_SZ] != '\t'))
        {
            break;
        }

        /* Prevents deleting [tab_width] lines at once with max. scrolled cursor
        in X. */
        if(remembered_line_idx != CURRENT_LINE_IDX)
        {
            break;
        }
    }
    SET_STATUS("edited");

    return true;
}

bool chars__tab(Buff_t* Buffer, const Conf_t* const Config)
{
    /* When the TAB key is pressed, it will insert e.g. 4 '\t' into the
    buffer. They will be converted during the rendering, loading and saving
    the file. */

    // Prevent not-full Tab insert.
    if(Buffer->chars_idx <= (CHARS_MAX - (idx_t) Config->Tab_sz.value))
    {
        for(int tab_sz = 0; tab_sz < Config->Tab_sz.value; tab_sz++)
        {
            if(!chars__printable_char(Buffer, '\t'))
            {
                return false;
            }
        }
    }
    else
    {
        SET_STATUS("can't read or insert more tabs");
    }
    return true;
}
