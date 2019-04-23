#include "edit.h"

bool edit__delete_char(Buff_t* Buffer)
{
    if(!CURSOR_AT_LINE_START)
    {
        edit__shift_text_horizonally(Buffer, 'l');
        if(!memory__shrink_current_line(Buffer))
        {
            return false;
        }
        CURRENT_LINE_LENGTH--;
        Buffer->chars_idx--;
    }
    // Deletes the non-empty line and copy chars to previous.
    else if(!FIRST_LINE)
    {
        if(!edit__move_lines_backward(Buffer))
        {
            return false;
        }
    }
    CURRENT_LINE[CURRENT_LINE_LENGTH] = '\0'; // Delete the linefeed.

    return true;
}

bool edit__delete_line(Buff_t* Buffer)
{
    idx_t next_line_idx    = CURRENT_LINE_IDX + NEXT;
    idx_t next_line_length = Buffer->lines_length[next_line_idx];

    if(!FIRST_LINE)
    {
        if(CURSOR_Y_SCROLLED)
        {
            Buffer->cursor_rev_x = (CURSOR_AT_LINE_START) ? next_line_length
                                   : LF_SZ;

            if(!memory__copy_lines_backward(Buffer))
            {
                return false;
            }
            if(!edit__delete_last_line(Buffer))
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

            /* With the last line deletion there is a need to remove the
            linefeed in the previous line. */
            LAST_LINE_LENGTH--;
            LAST_LINE[LAST_LINE_LENGTH] = '\0';

            Buffer->cursor_rev_x = 0;
        }
    }
    else
    {
        LAST_LINE_LENGTH = 0;
        LAST_LINE[LAST_LINE_LENGTH] = '\0';

        LAST_LINE = realloc(LAST_LINE, INITIAL_MEMBLOCK);
        if(LAST_LINE == NULL)
        {
            fprintf(stderr, "Can't realloc a memory in the first line.\n");
        }
        Buffer->cursor_rev_x = 0;
    }
    return true;
}

void edit__shift_text_horizonally(Buff_t* Buffer, const char direction)
{
    const size_t prev     = 1;
    idx_t        char_idx = CURSOR_X;

    switch(direction)
    {
    case 'l':
        for(; char_idx <= CURRENT_LINE_LENGTH; char_idx++)
        {
            CURRENT_LINE[char_idx - prev] = CURRENT_LINE[char_idx];
        }
        break;

    case 'r':
        for(char_idx = CURRENT_LINE_LENGTH; char_idx >= CURSOR_X;
            char_idx--)
        {
            CURRENT_LINE[char_idx] = CURRENT_LINE[char_idx - prev];
        }
    }
}

bool edit__move_lines_forward(Buff_t* Buffer)
{
    PREVIOUS_LINE_LENGTH -= Buffer->cursor_rev_x;

    // Move more lines vertically with the part of the current line.
    if(CURSOR_Y_SCROLLED)
    {
        if(!memory__copy_lines_forward(Buffer))
        {
            return false;
        }
        CURRENT_LINE_LENGTH = 0;
    }

    // Move the right part (separated by the cursor) of the line to the next.
    for(idx_t char_i = PREVIOUS_LINE_LENGTH;
        char_i < PREVIOUS_LINE_LENGTH + Buffer->cursor_rev_x; char_i++)
    {
        CURRENT_LINE[CURRENT_LINE_LENGTH] = PREVIOUS_LINE[char_i];
        CURRENT_LINE_LENGTH++;
        if(!memory__extend_line(Buffer, CURRENT_LINE_IDX))
        {
            return false;
        }
    }

    // Now the length of the upper line will be shortened after copying.
    PREVIOUS_LINE[PREVIOUS_LINE_LENGTH] = '\0';
    if(!memory__shrink_prev_line(Buffer))
    {
        return false;
    }
    return true;
}

bool edit__move_lines_backward(Buff_t* Buffer)
{
    Buffer->chars_idx--;
    PREVIOUS_LINE_LENGTH--;

    // Concat the previous line with the next.
    for(idx_t char_idx = 0; char_idx <= CURRENT_LINE_LENGTH; char_idx++)
    {
        PREVIOUS_LINE[PREVIOUS_LINE_LENGTH] = CURRENT_LINE[char_idx];

        if(CURRENT_LINE[char_idx] != '\0')
        {
            PREVIOUS_LINE_LENGTH++;
        }
        if(!memory__extend_line(Buffer, PREVIOUS_LINE_IDX))
        {
            return false;
        }
    }

    if(CURSOR_Y_SCROLLED)
    {
        if(!memory__copy_lines_backward(Buffer))
        {
            return false;
        }
    }
    if(!edit__delete_last_line(Buffer))
    {
        return false;
    }
    return true;
}

bool edit__delete_last_empty_line(Buff_t* Buffer)
{
    free(CURRENT_LINE);

    Buffer->lines_idx--;
    if(!memory__shrink_current_line(Buffer))
    {
        return false;
    }

    CURRENT_LINE_LENGTH--;
    Buffer->chars_idx--;

    if(!memory__shrink_lines_array(Buffer))
    {
        return false;
    }
    return true;
}

bool edit__delete_last_line(Buff_t* Buffer)
{
    free(LAST_LINE);

    Buffer->lines_idx--;
    return memory__shrink_lines_array(Buffer);
}

void edit__filename(Buff_t* Buffer, const Conf_t* const Config, Mod_t* Modes,
                    const char key)
{
    const char enter  = '\n';
    const char escape = CTRL_LEFT_BRACKET;

    if((key >= 32) && (key != BACKSPACE)
       && ((Buffer->fname_length + IDX) < PATH_MAX))
    {
        Buffer->fname[Buffer->fname_length] = key;
        Buffer->fname_length++;
        Buffer->fname[Buffer->fname_length] = '\0';
    }
    else if((key == BACKSPACE) && (Buffer->fname_length > 0))
    {
        Buffer->fname_length--;
        Buffer->fname[Buffer->fname_length] = '\0';
    }
    else if(key == enter)
    {
        Modes->live_fname_edit = false;
        SET_STATUS("filename edited and saved as");

        if(!strcmp(Buffer->fname, Buffer->fname_copy))
        {
            SET_STATUS("saved as");
        }
        file__save(Buffer, Config);
        strncpy(Buffer->fname_copy, Buffer->fname, PATH_MAX);
    }
    else if(key == escape)
    {
        strncpy(Buffer->fname, Buffer->fname_copy, PATH_MAX);
        Buffer->fname_length = strlen(Buffer->fname);

        Modes->live_fname_edit = false;
        SET_STATUS("filename unchanged");
    }
}
