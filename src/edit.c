#include "edit.h"

bool edit__delete_char(Buff_t* Buffer)
{
    if(!BUFFER__CURSOR_AT_LN_START)
    {
        edit__shift_text_horizonally(Buffer, 'l');
        if(!memory__shrink_current_line(Buffer))
        {
            return false;
        }
        BUFFER__ACTUAL_LN.len--;
        Buffer->ch_amount--;
    }
    // Deletes a non-empty line and copy chars to previous.
    else if(!BUFFER__FIRST_LN && !edit__move_lines_backward(Buffer))
    {
        return false;
    }
    BUFFER__LAST_CH_IN_LN = '\0'; // Delete the linefeed.

    return true;
}

bool edit__delete_line(Buff_t* Buffer)
{
    const size_t next_line_i   = BUFFER__ACTUAL_LN_I + SIZE__NEXT;
    const size_t next_line_len = Buffer->Lines[next_line_i].len;

    if(!BUFFER__FIRST_LN)
    {
        if(BUFFER__CURSOR_Y_SCROLLED)
        {
            Buffer->cursor_rev_x = (BUFFER__CURSOR_AT_LN_START)
                                   ? next_line_len : SIZE__LF;

            if(!memory__copy_lines_backward(Buffer)
               || !edit__delete_last_line(Buffer))
            {
                return false;
            }
            Buffer->cursor_rev_y--;
        }
        else
        {
            if(!edit__delete_last_line(Buffer))
            {
                return false;
            }

            /* With a last line deletion there is a need to remove the
               linefeed in a previous line. */
            BUFFER__LAST_LN.len--;
            BUFFER__LAST_LN.txt[BUFFER__LAST_LN.len] = '\0';

            Buffer->cursor_rev_x = 0;
        }
    }
    else
    {
        BUFFER__LAST_LN.len                      = 0;
        BUFFER__LAST_LN.txt[BUFFER__LAST_LN.len] = '\0';

        BUFFER__LAST_LN.txt = realloc(BUFFER__LAST_LN.txt,
                                      BUFFER__BASIC_MEMBLK);
        if(BUFFER__LAST_LN.txt == NULL)
        {
            fprintf(stderr, "Can't realloc a memory in a first line.\n");
        }
        Buffer->cursor_rev_x = 0;
    }
    return true;
}

void edit__shift_text_horizonally(Buff_t* Buffer, const char direction)
{
    size_t ch_i;
    char   tmp_ch;

    switch(direction)
    {
    case 'l':
        for(ch_i = BUFFER__CURSOR_X; ch_i <= BUFFER__ACTUAL_LN.len; ch_i++)
        {
            tmp_ch = BUFFER__ACTUAL_LN.txt[ch_i];
            BUFFER__ACTUAL_LN.txt[ch_i - SIZE__PREV] = tmp_ch;
        }
        break;

    case 'r':
        for(ch_i = BUFFER__ACTUAL_LN.len; ch_i >= BUFFER__CURSOR_X;
            ch_i--)
        {
            tmp_ch = BUFFER__ACTUAL_LN.txt[ch_i - SIZE__PREV];
            BUFFER__ACTUAL_LN.txt[ch_i] = tmp_ch;
        }
    }
}

bool edit__move_lines_forward(Buff_t* Buffer)
{
    BUFFER__PREV_LN.len -= Buffer->cursor_rev_x;

    // Move more lines vertically with a part of a current line.
    if(BUFFER__CURSOR_Y_SCROLLED)
    {
        if(!memory__copy_lines_forward(Buffer))
        {
            return false;
        }
        BUFFER__ACTUAL_LN.len = 0;
    }

    // Move a right part (separated by the cursor) of a line to a next.
    for(size_t ch_i = BUFFER__PREV_LN.len;
        ch_i < BUFFER__PREV_LN.len + Buffer->cursor_rev_x; ch_i++)
    {
        BUFFER__LAST_CH_IN_LN = BUFFER__PREV_LN.txt[ch_i];
        BUFFER__ACTUAL_LN.len++;
        if(!memory__extend_line(Buffer, BUFFER__ACTUAL_LN_I))
        {
            return false;
        }
    }

    // Now a length of an upper line will be shrinked after copying.
    BUFFER__PREV_LN.txt[BUFFER__PREV_LN.len] = '\0';
    if(!memory__shrink_prev_line(Buffer))
    {
        return false;
    }
    return true;
}

bool edit__move_lines_backward(Buff_t* Buffer)
{
    Buffer->ch_amount--;
    BUFFER__PREV_LN.len--;

    // Merge a previous line with a next.
    for(size_t ch_i = 0; ch_i <= BUFFER__ACTUAL_LN.len; ch_i++)
    {
        BUFFER__PREV_LN.txt[BUFFER__PREV_LN.len] = BUFFER__ACTUAL_LN.txt[ch_i];

        if(BUFFER__ACTUAL_LN.txt[ch_i] != '\0')
        {
            BUFFER__PREV_LN.len++;
        }
        if(!memory__extend_line(Buffer, BUFFER__PREV_LN_I))
        {
            return false;
        }
    }
    if(BUFFER__CURSOR_Y_SCROLLED && !memory__copy_lines_backward(Buffer))
    {
        return false;
    }
    if(!edit__delete_last_line(Buffer))
    {
        return false;
    }
    return true;
}

bool edit__delete_last_empty_line(Buff_t* Buffer)
{
    free(BUFFER__ACTUAL_LN.txt);

    Buffer->ln_amount--;
    if(!memory__shrink_current_line(Buffer))
    {
        return false;
    }
    BUFFER__ACTUAL_LN.len--;
    Buffer->ch_amount--;

    if(!memory__shrink_lines_array(Buffer))
    {
        return false;
    }
    return true;
}

bool edit__delete_last_line(Buff_t* Buffer)
{
    free(BUFFER__LAST_LN.txt);

    Buffer->ln_amount--;
    return memory__shrink_lines_array(Buffer);
}

void edit__filename(Buff_t* Buffer, const Conf_t* const Config, Mod_t* Modes,
                    const char key)
{
    const char escape = ASCII__CTRL_LEFT_BRACKET;

    if((key >= 32) && (key != ASCII__BACKSPACE)
       && ((Buffer->fname_len + SIZE__I) < PATH_MAX))
    {
        Buffer->fname[Buffer->fname_len] = key;
        Buffer->fname[++Buffer->fname_len] = '\0';
    }
    else if((key == ASCII__BACKSPACE) && (Buffer->fname_len > 0))
    {
        Buffer->fname[--Buffer->fname_len] = '\0';
    }
    else if(key == '\n')
    {
        Modes->live_fname_edit = false;
        BUFFER__SET_STATUS("saved as with a different name");

        if(!strcmp(Buffer->fname, Buffer->fname_copy))
        {
            BUFFER__SET_STATUS("saved as");
        }
        file_io__save(Buffer, Config);
        strncpy(Buffer->fname_copy, Buffer->fname, PATH_MAX);
    }
    else if(key == escape)
    {
        strncpy(Buffer->fname, Buffer->fname_copy, PATH_MAX);
        Buffer->fname_len = strlen(Buffer->fname);

        Modes->live_fname_edit = false;
        BUFFER__SET_STATUS("filename unchanged");
    }
}
