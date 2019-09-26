#include "chars.h"

bool chars__parse_char(V_file_t* V_file, const Conf_t* const Config,
                       Mod_t* const Modes, const char ch)
{
    switch(ch)
    {
    default:
        return chars__printable_char(V_file, ch);

    case '\t':
        return chars__tab(V_file, Config, Modes);

    case ASCII__BACKSPACE:
        return chars__backspace(V_file, Config, Modes);

    case ASCII__CTRL_Q:
        return false;

    case ASCII__CTRL_S:
        return file_io__save(V_file, Config);

    case ASCII__CTRL_BACKSLASH:
        Modes->expanded_lbar = !Modes->expanded_lbar;
        break;

    case ASCII__CTRL_D:
        return edit__delete_line(V_file);

    case ASCII__CTRL_O:
        Modes->live_fname_edit = true;
    }
    return true;
}

bool chars__linefeed(V_file_t* V_file)
{
    if(V_file->ln_amount < V_FILE__CH_MAX)
    {
        V_file->ln_amount++;
        if(!memory__extend_lines_array(V_file))
        {
            return false;
        }
        if(V_FILE__CURSOR_X_SCROLLED)
        {
            if(!edit__move_lines_forward(V_file))
            {
                return false;
            }
        }
        else if(V_FILE__CURSOR_Y_SCROLLED)
        {
            if(!memory__copy_lines_forward(V_file))
            {
                return false;
            }
        }
        V_FILE__LAST_CH_IN_LN = '\0';
    }
    return true;
}

bool chars__printable_char(V_file_t* const V_file, const char ch)
{

#ifdef DEBUG_KEYS
    const bool in_charmap = true;
#else
    const bool in_charmap = (ch == '\0') || (ch == '\t') || (ch == '\n')
                            || (ch >= 32);
#endif

    if(in_charmap)
    {
        if(V_FILE__CH_LIMIT_NOT_EXCEEDED)
        {
            V_file->ch_amount++;
            V_FILE__ACTUAL_LN.len++;

            if(!memory__extend_line(V_file, V_FILE__ACTUAL_LN_I))
            {
                return false;
            }
            if(V_FILE__CURSOR_X_SCROLLED)
            {
                edit__shift_text_horizonally(V_file, 'r');
            }
            V_FILE__ACTUAL_LN.txt[V_FILE__CURSOR_X - SIZE__NUL] = ch;
            V_FILE__LAST_CH_IN_LN                               = '\0';

            // Initializing nul handler.
            if((ch == '\0') && !V_FILE__EMPTY_LN)
            {
                V_file->ch_amount--;
                V_FILE__ACTUAL_LN.len--;
            }
            else if((ch == '\n') && !chars__linefeed(V_file))
            {
                return false;
            }
            if(ch != '\0')
            {
                V_FILE__SET_STATUS("edited");
            }
        }
        else
        {
            V_FILE__SET_STATUS("can't read or insert more chars");
        }
    }
    else
    {
        V_FILE__SET_STATUS("unsupported char(s)");
    }
    return true;
}

bool chars__backspace(V_file_t* V_file, const Conf_t* const Config,
                      const Mod_t* const Modes)
{
    const size_t ln_i_before_change = V_FILE__ACTUAL_LN_I;
    const char   tab_ch             = (Modes->tabs_to_spaces) ? ' ' : '\t';
    const size_t tab_sz             = (const size_t) Config->Tab_sz.value;

    for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
    {
        // Prevent removing a char and 3 tabs from that e.g.: "\t\t\t\t".
        if((V_FILE__CURSOR_X > 1)
           && (V_FILE__ACTUAL_LN.txt[V_FILE__CURSOR_X - SIZE__NUL - SIZE__PREV]
               == tab_ch)
           && (V_FILE__ACTUAL_LN.txt[V_FILE__CURSOR_X - SIZE__NUL]
               != tab_ch))
        {
            tab_i = (const size_t) Config->Tab_sz.value - SIZE__I;
        }

        /* Scenario when there is a char at the beginning and the Tab at the
           right. */
        if((V_FILE__CURSOR_X == 1) && (V_file->cursor_rev_x > 0)
           && (V_FILE__ACTUAL_LN.txt[V_FILE__CURSOR_X - SIZE__NUL] != tab_ch)
           && (V_FILE__ACTUAL_LN.txt[V_FILE__CURSOR_X] == tab_ch))
        {
            if(!edit__delete_char(V_file))
            {
                return false;
            }
            break;
        }
        if(!V_FILE__EMPTY_LN)
        {
            if(!edit__delete_char(V_file))
            {
                return false;
            }
        }
        else if(!V_FILE__FIRST_LN && !V_FILE__CURSOR_Y_SCROLLED
                && !edit__delete_last_empty_line(V_file))
        {
            return false;
        }

        // Some text and the Tab(s) at the end.
        if((V_FILE__ACTUAL_LN.len > 0) && (V_file->cursor_rev_x == 0)
           && (V_FILE__ACTUAL_LN.txt[V_FILE__ACTUAL_LN.len - SIZE__NUL]
               != tab_ch))
        {
            break;
        }
        /* Prevent removing a line when a first char in a line has to be
           removed. */
        else if((V_FILE__CURSOR_X == 0)
                && (V_FILE__ACTUAL_LN.txt[V_FILE__CURSOR_X] != tab_ch))
        {
            break;
        }
        // Scenario when there is the Tab and some text further.
        else if((V_FILE__CURSOR_X > 0) && (V_file->cursor_rev_x > 0)
                && (V_FILE__ACTUAL_LN.txt[V_FILE__CURSOR_X - SIZE__NUL]
                    != tab_ch))
        {
            break;
        }

        /* Prevents deleting [tab_width] lines at once with a maximally
           scrolled cursor in X. */
        if(ln_i_before_change != V_FILE__ACTUAL_LN_I)
        {
            break;
        }
    }
    V_FILE__SET_STATUS("edited");

    return true;
}

bool chars__tab(V_file_t* V_file, const Conf_t* const Config,
                const Mod_t* const Modes)
{
    /* When the Tab key is pressed, it will insert e.g. 4 '\t' into a buffer.
       They will be converted during a rendering, loading and saving a file. */

    const size_t tab_sz = (const size_t) Config->Tab_sz.value;
    const char   tab_ch = (Modes->tabs_to_spaces) ? ' ' : '\t';

    // Prevent the not-full Tab insert.
    if(V_file->ch_amount <= (const size_t) (V_FILE__CH_MAX - tab_sz))
    {
        for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
        {
            if(!chars__printable_char(V_file, tab_ch))
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
