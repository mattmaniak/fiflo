#include "buffer.h"
#include "config.h"
#include "modes.h"
#include "shortcuts.h"
#include "keys.h"

bool keys__key_action(Buff_t* Buffer, Conf_t* Config, Mod_t* Modes, char key)
{
    switch(key)
    {
    default:
        return keys__printable_char(Buffer, key);

    case '\t':
        return keys__tab(Buffer, Config);

    case BACKSPACE:
        return keys__backspace(Buffer, Config);

    case CTRL_Q:
        return false;

    case CTRL_S:
        if(!file__save(Buffer, Config))
        {
            return false;
        }
        break;

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

bool keys__linefeed(Buff_t* Buffer)
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
        CURRENT_LINE[CURRENT_LINE_LENGTH_IDX] = '\0';
    }
    return true;
}

bool keys__printable_char(Buff_t* Buffer, char key)
{

#ifdef DEBUG_KEYS
    const bool in_keymap = true;
#else
    const bool in_keymap = (key == '\0') || (key == '\t') || (key == '\n')
                           || (key >= 32);
#endif

    if(in_keymap)
    {
        if(BUFFER_NOT_FULL)
        {
            Buffer->chars_idx++;
            CURRENT_LINE_LENGTH_IDX++;

            if(!memory__extend_line(Buffer, CURRENT_LINE_IDX))
            {
                return false;
            }

            if(CURSOR_X_SCROLLED)
            {
                edit__shift_text_horizonally(Buffer, 'r');
            }
            CURRENT_LINE[CURSOR_X_IDX - NUL_SZ]       = key;
            CURRENT_LINE[CURRENT_LINE_LENGTH_IDX] = '\0';

            // Initializing nul handler.
            if((key == '\0') && !EMPTY_LINE)
            {
                Buffer->chars_idx--;
                CURRENT_LINE_LENGTH_IDX--;
            }
            else if(key == '\n')
            {
                if(!keys__linefeed(Buffer))
                {
                    return false;
                }
            }
            if(key != '\0')
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

bool keys__backspace(Buff_t* Buffer, Conf_t* Config)
{
    const idx_t prev = 1;
    idx_t       remembered_line_idx = CURRENT_LINE_IDX;

    for(idx_t tab_idx = 0; tab_idx < (idx_t) Config->Tab_width.value; tab_idx++)
    {
        // Prevent removind char and 3 tabs from that e.g.: "\t\t\t\t".
        if((CURSOR_X_IDX > 1)
           && (CURRENT_LINE[CURSOR_X_IDX - prev - NUL_SZ] == '\t')
           && (CURRENT_LINE[CURSOR_X_IDX - NUL_SZ] != '\t'))
        {
            tab_idx = (idx_t) Config->Tab_width.value - IDX;
        }

        // Scenario when there is char at the beginning and tab at the right.
        if((CURSOR_X_IDX == 1) && (CURRENT_LINE[CURSOR_X_IDX - NUL_SZ] != '\t')
           && (CURRENT_LINE[CURSOR_X_IDX] == '\t')
           && (Buffer->cursor_rev_x > 0))
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
        if((CURRENT_LINE_LENGTH_IDX > 0) && (Buffer->cursor_rev_x == 0)
           && (CURRENT_LINE[CURRENT_LINE_LENGTH_IDX - NUL_SZ] != '\t'))
        {
            break;
        }
        /* Prevent removing the line when the first char in the line has to be
        removed. */
        else if((CURSOR_X_IDX == 0) && (CURRENT_LINE[CURSOR_X_IDX] != '\t'))
        {
            break;
        }
        // Scenario when there is the tab and some text further.
        else if((CURSOR_X_IDX > 0)
                && (CURRENT_LINE[CURSOR_X_IDX - NUL_SZ] != '\t')
                && (Buffer->cursor_rev_x > 0))
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

bool keys__tab(Buff_t* Buffer, Conf_t* Config)
{
    /* When the TAB key is pressed, it will insert e.g. 4 '\t' into the
    buffer. They will be converted during the rendering, loading and saving
    the file. */

    // Prevent not-full Tab insert.
    if(Buffer->chars_idx <= (CHARS_MAX - (idx_t) Config->Tab_width.value))
    {
        for(int tab_sz = 0; tab_sz < Config->Tab_width.value; tab_sz++)
        {
            if(!keys__printable_char(Buffer, '\t'))
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
