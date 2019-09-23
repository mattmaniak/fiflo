#include "edit.h"

bool edit__delete_char(Buff_t* Buffer)
{
    if(!BUFFER__CURSOR_AT_LINE_BEGINNING)
    {
        edit__shift_text_horizonally(Buffer, 'l');
        if(!memory__shrink_current_line(Buffer))
        {
            return false;
        }
        BUFFER__ACTUAL_LINE.len--;
        Buffer->chars_amount--;
    }
    // Deletes a non-empty line and copy chars to previous.
    else if(!BUFFER__FIRST_LINE && !edit__move_lines_backward(Buffer))
    {
        return false;
    }
    BUFFER__LAST_CHAR_IN_LINE = '\0'; // Delete the linefeed.

    return true;
}

bool edit__delete_line(Buff_t* Buffer)
{
    const idx_t next_line_idx    = BUFFER__ACTUAL_LINE_IDX + SIZE__NEXT;
    const idx_t next_line_length = Buffer->Lines[next_line_idx].len;

    if(!BUFFER__FIRST_LINE)
    {
        if(BUFFER__CURSOR_Y_SCROLLED)
        {
            Buffer->cursor_rev_x = (BUFFER__CURSOR_AT_LINE_BEGINNING)
                                   ? next_line_length : SIZE__LF;

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
            BUFFER__LAST_LINE.len--;
            BUFFER__LAST_LINE.txt[BUFFER__LAST_LINE.len] = '\0';

            Buffer->cursor_rev_x = 0;
        }
    }
    else
    {
        BUFFER__LAST_LINE.len = 0;
        BUFFER__LAST_LINE.txt[BUFFER__LAST_LINE.len] = '\0';

        BUFFER__LAST_LINE.txt = realloc(BUFFER__LAST_LINE.txt, BUFFER__BASIC_MEMBLK);
        if(BUFFER__LAST_LINE.txt == NULL)
        {
            fprintf(stderr, "Can't realloc a memory in a first line.\n");
        }
        Buffer->cursor_rev_x = 0;
    }
    return true;
}

void edit__shift_text_horizonally(Buff_t* Buffer, const char direction)
{
    idx_t ch_idx;

    switch(direction)
    {
    case 'l':
        for(ch_idx = BUFFER__CURSOR_X; ch_idx <= BUFFER__ACTUAL_LINE.len;
            ch_idx++)
        {
            BUFFER__ACTUAL_LINE.txt[ch_idx - SIZE__PREV] =
            BUFFER__ACTUAL_LINE.txt[ch_idx];
        }
        break;

    case 'r':
        for(ch_idx = BUFFER__ACTUAL_LINE.len; ch_idx >= BUFFER__CURSOR_X;
            ch_idx--)
        {
            BUFFER__ACTUAL_LINE.txt[ch_idx] =
            BUFFER__ACTUAL_LINE.txt[ch_idx - SIZE__PREV];
        }
    }
}

bool edit__move_lines_forward(Buff_t* Buffer)
{
    BUFFER__PREV_LINE.len -= Buffer->cursor_rev_x;

    // Move more lines vertically with a part of a current line.
    if(BUFFER__CURSOR_Y_SCROLLED)
    {
        if(!memory__copy_lines_forward(Buffer))
        {
            return false;
        }
        BUFFER__ACTUAL_LINE.len = 0;
    }

    // Move a right part (separated by the cursor) of a line to a next.
    for(idx_t ch_idx = BUFFER__PREV_LINE.len;
        ch_idx < BUFFER__PREV_LINE.len + Buffer->cursor_rev_x; ch_idx++)
    {
        BUFFER__LAST_CHAR_IN_LINE = BUFFER__PREV_LINE.txt[ch_idx];
        BUFFER__ACTUAL_LINE.len++;
        if(!memory__extend_line(Buffer, BUFFER__ACTUAL_LINE_IDX))
        {
            return false;
        }
    }

    // Now a length of an upper line will be shrinked after copying.
    BUFFER__PREV_LINE.txt[BUFFER__PREV_LINE.len] = '\0';
    if(!memory__shrink_prev_line(Buffer))
    {
        return false;
    }
    return true;
}

bool edit__move_lines_backward(Buff_t* Buffer)
{
    Buffer->chars_amount--;
    BUFFER__PREV_LINE.len--;

    // Merge a previous line with a next.
    for(idx_t ch_idx = 0; ch_idx <= BUFFER__ACTUAL_LINE.len; ch_idx++)
    {
        BUFFER__PREV_LINE.txt[BUFFER__PREV_LINE.len] =
        BUFFER__ACTUAL_LINE.txt[ch_idx];

        if(BUFFER__ACTUAL_LINE.txt[ch_idx] != '\0')
        {
            BUFFER__PREV_LINE.len++;
        }
        if(!memory__extend_line(Buffer, BUFFER__PREV_LINE_IDX))
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
    free(BUFFER__ACTUAL_LINE.txt);

    Buffer->lines_amount--;
    if(!memory__shrink_current_line(Buffer))
    {
        return false;
    }

    BUFFER__ACTUAL_LINE.len--;
    Buffer->chars_amount--;

    if(!memory__shrink_lines_array(Buffer))
    {
        return false;
    }
    return true;
}

bool edit__delete_last_line(Buff_t* Buffer)
{
    free(BUFFER__LAST_LINE.txt);

    Buffer->lines_amount--;
    return memory__shrink_lines_array(Buffer);
}

void edit__filename(Buff_t* Buffer, const Conf_t* const Config, Mod_t* Modes,
                    const char key)
{
    const char enter  = '\n';
    const char escape = KEYS__CTRL_LEFT_BRACKET;

    if((key >= 32) && (key != KEYS__BACKSPACE)
       && ((Buffer->fname_len + SIZE__IDX) < PATH_MAX))
    {
        Buffer->fname[Buffer->fname_len] = key;
        Buffer->fname[++Buffer->fname_len] = '\0';
    }
    else if((key == KEYS__BACKSPACE) && (Buffer->fname_len > 0))
    {
        Buffer->fname[--Buffer->fname_len] = '\0';
    }
    else if(key == enter)
    {
        Modes->live_fname_edit = false;
        BUFFER__SET_STATUS("saved as with a different name");

        if(!strcmp(Buffer->fname, Buffer->fname_copy))
        {
            BUFFER__SET_STATUS("saved as");
        }
        file__save(Buffer, Config);
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
