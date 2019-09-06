#include "print.h"

void print__line_with_tabs(const Buff_t* const Buffer,
                           const Conf_t* const Config, const idx_t line_idx,
                           const idx_t start_char_idx,
                           const idx_t end_char_idx)
{
    for(idx_t char_idx = start_char_idx; char_idx < end_char_idx; char_idx++)
    {
        const char ch = Buffer->Lines[line_idx].text[char_idx];

        // Whitespace highlighting.
        if(ch == '\t')
        {
            ui__set_color(&Config->Color_whitespace);
            putchar(PRINT__TAB_HIGHLIGHT);
        }
        else if(ch == ' ')
        {
            ui__set_color(&Config->Color_whitespace);
            putchar(PRINT__SPACE_HIGHLIGHT);
        }
        else
        {
            const idx_t char_idx_after_highlighting =
            highlighter__paint_word(&Buffer->Lines[line_idx], char_idx);

            if(char_idx == char_idx_after_highlighting)
            {
                ui__set_color(&Config->Color_text);
                putchar(ch);
            }
            else // Word printed and highlighted. Shift the index to the next.
            {
                char_idx = char_idx_after_highlighting;
            }
        }
        ui__set_color(NULL);
    }
}

idx_t print__set_start_line(const Buff_t* const Buffer, const Ui_t* const Ui)
{
    idx_t scrolled_lines = 0;

    if(BUFFER__CURRENT_LINE_IDX >= Ui->textarea_h)
    {
        // Amount of lines to hide in the magic upper area.
        scrolled_lines = Buffer->lines_amount_idx + IDX - Ui->textarea_h
                         - Buffer->cursor_rev_y;
    }
    return scrolled_lines;
}

void print__actual_line(const Buff_t* const Buffer, const Conf_t* const Config,
                        const Ui_t* const Ui)
{
    // There is small amount of chars. Horizontal scroll isn't required.
    if(BUFFER__CURRENT_LINE_LEN < Ui->textarea_w)
    {
        print__line_with_tabs(Buffer, Config, BUFFER__CURRENT_LINE_IDX, 0,
                              BUFFER__CURRENT_LINE_LEN);
    }
    // Chars won't fits in the horizontal space.
    else if((BUFFER__CURRENT_LINE_LEN - Ui->textarea_w)
            >= Buffer->cursor_rev_x)
    {
        // Render only right part of the line.
        print__scroll_line_horizontally(Buffer, Config, Ui);
    }
    else // Shrink the line.
    {
        print__line_with_tabs(Buffer, Config, BUFFER__CURRENT_LINE_IDX, 0,
                              (idx_t) Ui->textarea_w - LF_SZ);

        // Not last rendered line so wrap it.
        if(((BUFFER__CURRENT_LINE_IDX + IDX) < Ui->textarea_h)
           && (BUFFER__CURRENT_LINE_IDX != Buffer->lines_amount_idx))
        {
            WRAP_LINE();
        }
    }
}

void print__another_line(const Buff_t* const Buffer, const Ui_t* const Ui,
                         const Conf_t* const Config, const idx_t line_idx)
{
    ui__print_line_number(Buffer, Config, line_idx, Ui->line_num_length);

    if(Buffer->Lines[line_idx].length < Ui->textarea_w)
    {
        print__line_with_tabs(Buffer, Config, line_idx, 0,
                              Buffer->Lines[line_idx].length);
    }
    else
    {
        print__line_with_tabs(Buffer, Config, line_idx, 0,
                              (idx_t) Ui->textarea_w - LF_SZ);
        WRAP_LINE();
    }
}

void print__scroll_line_horizontally(const Buff_t* const Buffer,
                                     const Conf_t* const Config,
                                     const Ui_t* const Ui)
{
    // Text will be scrolled. Not cursor.
    print__line_with_tabs(Buffer, Config, BUFFER__CURRENT_LINE_IDX,
                          BUFFER__CURSOR_X + CURSOR_SZ - Ui->textarea_w,
                          BUFFER__CURSOR_X);

    /* Sometimes is needed because the "window__fill" function renders the
    smallest required amount of linefeeds. In other cases the linefeed is
    provided by the char in a line. */
    if(BUFFER__CURSOR_Y_SCROLLED)
    {
        WRAP_LINE();
    }
}

void print__fit_lines(const Buff_t* const Buffer, const Ui_t* const Ui,
                      const Conf_t* const Config)
{
    const idx_t current_line_sz = 1;
    idx_t       line_idx;

    for(line_idx = 0; line_idx < BUFFER__CURRENT_LINE_IDX; line_idx++)
    {
        print__another_line(Buffer, Ui, Config, line_idx);
    }
    ui__print_line_number(Buffer, Config, BUFFER__CURRENT_LINE_IDX,
                          Ui->line_num_length);

    print__actual_line(Buffer, Config, Ui);

    if(BUFFER__CURSOR_Y_SCROLLED)
    {
        for(line_idx += current_line_sz; line_idx < Buffer->lines_amount_idx;
            line_idx++)
        {
            print__another_line(Buffer, Ui, Config, line_idx);
        }
        ui__print_line_number(Buffer, Config, Buffer->lines_amount_idx,
                              Ui->line_num_length);


        if(Buffer->Lines[Buffer->lines_amount_idx].length < Ui->textarea_w)
        {
            print__line_with_tabs(Buffer, Config, Buffer->lines_amount_idx, 0,
                                  Buffer->Lines[Buffer->lines_amount_idx].
                                  length);
        }
        else
        {
            print__line_with_tabs(Buffer, Config, Buffer->lines_amount_idx, 0,
                                  (idx_t) Ui->textarea_w - LF_SZ);
        }
    }
}

void print__shrink_lines(const Buff_t* const Buffer, const Ui_t* const Ui,
                         const Conf_t* const Config)
{
    const idx_t last_line_idx = (const idx_t) Ui->textarea_h - IDX;
    idx_t       line_idx      = 0;

    // Previous lines. If scrolled. Only beginning is shown.
    for(; line_idx < BUFFER__CURRENT_LINE_IDX; line_idx++)
    {
        print__another_line(Buffer, Ui, Config, line_idx);
    }
    ui__print_line_number(Buffer, Config, BUFFER__CURRENT_LINE_IDX,
                          Ui->line_num_length);

    print__actual_line(Buffer, Config, Ui);

    // Next lines. If scrolled. Only beginning is shown.
    for(line_idx = BUFFER__CURRENT_LINE_IDX + IDX; line_idx < last_line_idx;
        line_idx++)
    {
        print__another_line(Buffer, Ui, Config, line_idx);
    }
    ui__print_line_number(Buffer, Config, last_line_idx, Ui->line_num_length);

    if(Buffer->Lines[last_line_idx].length < Ui->textarea_w)
    {
        print__line_with_tabs(Buffer, Config, last_line_idx, 0,
                              Buffer->Lines[last_line_idx].length - LF_SZ);
    }
    else
    {
        print__line_with_tabs(Buffer, Config, last_line_idx, 0,
                              (idx_t) Ui->textarea_w - LF_SZ);
    }
}

void print__scroll_lines(const Buff_t* const Buffer, const Ui_t* const Ui,
                         const Conf_t* const Config)
{
    idx_t chars_end_offset = BUFFER__CURRENT_LINE_LEN;

    // Previous lines. If scrolled. Only beginning is shown.
    for(idx_t line_idx = print__set_start_line(Buffer, Ui);
        line_idx < BUFFER__CURRENT_LINE_IDX; line_idx++)
    {
        print__another_line(Buffer, Ui, Config, line_idx);
    }

    // Display the last line without the linefeed.
    ui__print_line_number(Buffer, Config, BUFFER__CURRENT_LINE_IDX,
                          Ui->line_num_length);

    if(BUFFER__CURRENT_LINE_LEN < Ui->textarea_w)
    {
        if((BUFFER__CURRENT_LINE_LEN > 0)
           && (BUFFER__CURRENT_LINE[BUFFER__CURRENT_LINE_LEN - NUL_SZ] \
           == '\n'))
        {
            chars_end_offset--;
        }
        print__line_with_tabs(Buffer, Config, BUFFER__CURRENT_LINE_IDX, 0,
                              chars_end_offset);

    }
    // Chars won't fits in the horizontal space.
    else if((BUFFER__CURRENT_LINE_LEN - Ui->textarea_w)
            >= Buffer->cursor_rev_x)
    {
        // Text will be scrolled. Not cursor.
        print__line_with_tabs(Buffer, Config, BUFFER__CURRENT_LINE_IDX,
                              BUFFER__CURSOR_X + CURSOR_SZ - Ui->textarea_w,
                              BUFFER__CURSOR_X);
    }
    else
    {
        // Render only left part of the line. Cursor can scrolled.
        print__line_with_tabs(Buffer, Config, BUFFER__CURRENT_LINE_IDX, 0,
                              (idx_t) Ui->textarea_w - LF_SZ);
    }
}

void print__display_text(const Buff_t* const Buffer, const Ui_t* const Ui,
                         const Conf_t* const Config)
{
    if(Buffer->lines_amount_idx < Ui->textarea_h)
    {
        print__fit_lines(Buffer, Ui, Config);
    }
    else if((BUFFER__CURRENT_LINE_IDX + IDX) < Ui->textarea_h)
    {
        print__shrink_lines(Buffer, Ui, Config);
    }
    else
    {
        // first_rendered_line_idx = ;
        print__scroll_lines(Buffer, Ui, Config);
    }
}
