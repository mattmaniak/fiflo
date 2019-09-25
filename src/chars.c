#include "chars.h"

bool chars__parse_char(Buff_t* Buffer, const Conf_t* const Config,
                       Mod_t* const Modes, const char ch)
{
    switch(ch)
    {
    default:
        return chars__printable_char(Buffer, ch);

    case '\t':
        return chars__tab(Buffer, Config, Modes);

    case ASCII__BACKSPACE:
        return chars__backspace(Buffer, Config, Modes);

    case ASCII__CTRL_Q:
        return false;

    case ASCII__CTRL_S:
        return file_io__save(Buffer, Config);

    case ASCII__CTRL_BACKSLASH:
        Modes->expanded_lbar = !Modes->expanded_lbar;
        break;

    case ASCII__CTRL_D:
        return edit__delete_line(Buffer);

    case ASCII__CTRL_O:
        Modes->live_fname_edit = true;
    }
    return true;
}

bool chars__linefeed(Buff_t* Buffer)
{
    if(Buffer->ln_amount < CHARS_MAX)
    {
        Buffer->ln_amount++;
        if(!memory__extend_lines_array(Buffer))
        {
            return false;
        }
        if(BUFFER__CURSOR_X_SCROLLED)
        {
            if(!edit__move_lines_forward(Buffer))
            {
                return false;
            }
        }
        else if(BUFFER__CURSOR_Y_SCROLLED)
        {
            if(!memory__copy_lines_forward(Buffer))
            {
                return false;
            }
        }
        BUFFER__LAST_CH_IN_LN = '\0';
    }
    return true;
}

bool chars__printable_char(Buff_t* const Buffer, const char ch)
{

#ifdef DEBUG_KEYS
    const bool in_charmap = true;
#else
    const bool in_charmap = (ch == '\0') || (ch == '\t') || (ch == '\n')
                            || (ch >= 32);
#endif

    if(in_charmap)
    {
        if(BUFFER__CH_LIMIT_NOT_EXCEEDED)
        {
            Buffer->ch_amount++;
            BUFFER__ACTUAL_LN.len++;

            if(!memory__extend_line(Buffer, BUFFER__ACTUAL_LN_I))
            {
                return false;
            }
            if(BUFFER__CURSOR_X_SCROLLED)
            {
                edit__shift_text_horizonally(Buffer, 'r');
            }
            BUFFER__ACTUAL_LN.txt[BUFFER__CURSOR_X - SIZE__NUL] = ch;
            BUFFER__LAST_CH_IN_LN                               = '\0';

            // Initializing nul handler.
            if((ch == '\0') && !BUFFER__EMPTY_LN)
            {
                Buffer->ch_amount--;
                BUFFER__ACTUAL_LN.len--;
            }
            else if((ch == '\n') && !chars__linefeed(Buffer))
            {
                return false;
            }
            if(ch != '\0')
            {
                BUFFER__SET_STATUS("edited");
            }
        }
        else
        {
            BUFFER__SET_STATUS("can't read or insert more chars");
        }
    }
    else
    {
        BUFFER__SET_STATUS("unsupported char(s)");
    }
    return true;
}

bool chars__backspace(Buff_t* Buffer, const Conf_t* const Config,
                      const Mod_t* const Modes)
{
    const size_t ln_i_before_operation = BUFFER__ACTUAL_LN_I;
    const char  tab_ch                = (Modes->tabs_to_spaces) ? ' ' : '\t';
    const size_t tab_sz                = (const size_t) Config->Tab_sz.value;

    for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
    {
        // Prevent removing a char and 3 tabs from that e.g.: "\t\t\t\t".
        if((BUFFER__CURSOR_X > 1)
           && (BUFFER__ACTUAL_LN.txt[BUFFER__CURSOR_X
                                       - SIZE__NUL - SIZE__PREV] == tab_ch)
           && (BUFFER__ACTUAL_LN.txt[BUFFER__CURSOR_X - SIZE__NUL]
               != tab_ch))
        {
            tab_i = (size_t) Config->Tab_sz.value - SIZE__I;
        }

        /* Scenario when there is a char at the beginning and the Tab at the
           right. */
        if((BUFFER__CURSOR_X == 1) && (Buffer->cursor_rev_x > 0)
           && (BUFFER__ACTUAL_LN.txt[BUFFER__CURSOR_X - SIZE__NUL] != tab_ch)
           && (BUFFER__ACTUAL_LN.txt[BUFFER__CURSOR_X] == tab_ch))
        {
            if(!edit__delete_char(Buffer))
            {
                return false;
            }
            break;
        }
        if(!BUFFER__EMPTY_LN)
        {
            if(!edit__delete_char(Buffer))
            {
                return false;
            }
        }
        else if(!BUFFER__FIRST_LN && !BUFFER__CURSOR_Y_SCROLLED &&
                !edit__delete_last_empty_line(Buffer))
        {
            return false;
        }

        // Some text and the Tab(s) at the end.
        if((BUFFER__ACTUAL_LN.len > 0) && (Buffer->cursor_rev_x == 0)
           && (BUFFER__ACTUAL_LN.txt[BUFFER__ACTUAL_LN.len - SIZE__NUL]
           != tab_ch))
        {
            break;
        }
        /* Prevent removing a line when a first char in a line has to be
           removed. */
        else if((BUFFER__CURSOR_X == 0)
                && (BUFFER__ACTUAL_LN.txt[BUFFER__CURSOR_X] != tab_ch))
        {
            break;
        }
        // Scenario when there is the Tab and some text further.
        else if((BUFFER__CURSOR_X > 0) && (Buffer->cursor_rev_x > 0)
                && (BUFFER__ACTUAL_LN.txt[BUFFER__CURSOR_X - SIZE__NUL]
                    != tab_ch))
        {
            break;
        }

        /* Prevents deleting [tab_width] lines at once with a maximally
           scrolled cursor in X. */
        if(ln_i_before_operation != BUFFER__ACTUAL_LN_I)
        {
            break;
        }
    }
    BUFFER__SET_STATUS("edited");

    return true;
}

bool chars__tab(Buff_t* Buffer, const Conf_t* const Config,
                const Mod_t* const Modes)
{
    /* When the Tab key is pressed, it will insert e.g. 4 '\t' into a buffer.
       They will be converted during a rendering, loading and saving a file. */

    const size_t tab_sz = (const size_t) Config->Tab_sz.value;
    const char  tab_ch = (Modes->tabs_to_spaces) ? ' ' : '\t';

    // Prevent the not-full Tab insert.
    if(Buffer->ch_amount <= (const size_t) (CHARS_MAX - tab_sz))
    {
        for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
        {
            if(!chars__printable_char(Buffer, tab_ch))
            {
                return false;
            }
        }
    }
    else
    {
        BUFFER__SET_STATUS("can't read or insert more tabs");
    }
    return true;
}
