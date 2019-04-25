#include "print.h"

void print__line_with_tabs(const Buff_t* const Buffer,
                           const Conf_t* const Config, const idx_t line_idx,
                           const idx_t start_char_idx, const idx_t end_char_idx)
{
    for(idx_t char_idx = start_char_idx; char_idx < end_char_idx; char_idx++)
    {
        // Whitespace highlighting.
        if(Buffer->text[line_idx][char_idx] == '\t')
        {
            ui__set_color(&Config->Color_whitespace);
            putchar(TAB_HIGHLIGHT);
        }
        else if(Buffer->text[line_idx][char_idx] == ' ')
        {
            ui__set_color(&Config->Color_whitespace);
            putchar(SPACE_HIGHLIGHT);
        }
        else
        {
            ui__set_color(&Config->Color_text);
            putchar(Buffer->text[line_idx][char_idx]);
        }
        ui__set_color(NULL);
    }
}

idx_t print__set_start_line(const Buff_t* const Buffer, const Ui_t* const Ui)
{
    idx_t scrolled_lines = 0;

    if(CURRENT_LINE_IDX >=  Ui->text_y)
    {
        // Amount of lines to hide in the magic upper area.
        scrolled_lines = Buffer->lines_idx + IDX - Ui->text_y
                         - Buffer->cursor_rev_y;
    }
    return scrolled_lines;
}

void print__actual_line(const Buff_t* const Buffer, const Conf_t* const Config,
                        const Ui_t* const Ui)
{
    // There is small amount of chars. Horizontal scroll isn't required.
    if(CURRENT_LINE_LENGTH < Ui->text_x)
    {
        print__line_with_tabs(Buffer, Config, CURRENT_LINE_IDX, 0,
                              CURRENT_LINE_LENGTH);
    }
    // Chars won't fits in the horizontal space.
    else if((CURRENT_LINE_LENGTH - Ui->text_x) >= Buffer->cursor_rev_x)
    {
        // Render only right part of the line.
        print__scroll_line_horizontally(Buffer, Config, Ui);
    }
    else // Shrink the line.
    {
        print__line_with_tabs(Buffer, Config, CURRENT_LINE_IDX, 0,
                              (idx_t) Ui->text_x - LF_SZ);

        // Not last rendered line so wrap it.
        if(((CURRENT_LINE_IDX + IDX) < Ui->text_y)
           && (CURRENT_LINE_IDX != Buffer->lines_idx))
        {
            WRAP_LINE();
        }
    }
}

void print__another_line(const Buff_t* const Buffer, const Ui_t* const Ui,
                         const Conf_t* const Config, const idx_t line_idx)
{
    ui__print_line_number(Buffer, Config, line_idx, Ui->line_num_length);

    if(Buffer->lines_length[line_idx] < Ui->text_x)
    {
        print__line_with_tabs(Buffer, Config, line_idx, 0,
                              Buffer->lines_length[line_idx]);
    }
    else
    {
        print__line_with_tabs(Buffer, Config, line_idx, 0,
                              (idx_t) Ui->text_x - LF_SZ);
        WRAP_LINE();
    }
}

void print__scroll_line_horizontally(const Buff_t* const Buffer,
                                     const Conf_t* const Config,
                                     const Ui_t* const Ui)
{
    // Text will be scrolled. Not cursor.
    print__line_with_tabs(Buffer, Config, CURRENT_LINE_IDX,
                          CURSOR_X + CURSOR_SZ - Ui->text_x, CURSOR_X);

    /* Sometimes is needed because the "window__fill" function renders the
    smallest required amount of linefeeds. In other cases the linefeed is
    provided by the char in a line. */
    if(CURSOR_Y_SCROLLED)
    {
        WRAP_LINE();
    }
}

void print__fit_lines(const Buff_t* const Buffer, const Ui_t* const Ui,
                      const Conf_t* const Config)
{
    const idx_t current_line_sz = 1;
    idx_t       line_idx;

    for(line_idx = 0; line_idx < CURRENT_LINE_IDX; line_idx++)
    {
        print__another_line(Buffer, Ui, Config, line_idx);
    }
    ui__print_line_number(Buffer, Config, CURRENT_LINE_IDX,
                          Ui->line_num_length);

    print__actual_line(Buffer, Config, Ui);

    if(CURSOR_Y_SCROLLED)
    {
        for(line_idx += current_line_sz; line_idx < Buffer->lines_idx;
            line_idx++)
        {
            print__another_line(Buffer, Ui, Config, line_idx);
        }
        ui__print_line_number(Buffer, Config, Buffer->lines_idx,
                              Ui->line_num_length);


        if(Buffer->lines_length[Buffer->lines_idx] < Ui->text_x)
        {
            print__line_with_tabs(Buffer, Config, Buffer->lines_idx, 0,
                                  Buffer->lines_length[Buffer->lines_idx]);
        }
        else
        {
            print__line_with_tabs(Buffer, Config, Buffer->lines_idx, 0,
                                  (idx_t) Ui->text_x - LF_SZ);
        }
    }
}

void print__shrink_lines(const Buff_t* const Buffer, const Ui_t* const Ui,
                         const Conf_t* const Config)
{
    idx_t last_line_idx = (idx_t) Ui->text_y - IDX;
    idx_t line_idx      = 0;

    // Previous lines. If scrolled. Only beginning is shown.
    for(; line_idx < CURRENT_LINE_IDX; line_idx++)
    {
        print__another_line(Buffer, Ui, Config, line_idx);
    }
    ui__print_line_number(Buffer, Config, CURRENT_LINE_IDX,
                          Ui->line_num_length);

    print__actual_line(Buffer, Config, Ui);

    // Next lines. If scrolled. Only beginning is shown.
    for(line_idx = CURRENT_LINE_IDX + IDX; line_idx < last_line_idx; line_idx++)
    {
        print__another_line(Buffer, Ui, Config, line_idx);
    }
    ui__print_line_number(Buffer, Config, last_line_idx, Ui->line_num_length);

    if(Buffer->lines_length[last_line_idx] < Ui->text_x)
    {
        print__line_with_tabs(Buffer, Config, last_line_idx, 0,
                              Buffer->lines_length[last_line_idx] - LF_SZ);
    }
    else
    {
        print__line_with_tabs(Buffer, Config, last_line_idx, 0,
                              (idx_t) Ui->text_x - LF_SZ);
    }
}

void print__scroll_lines(const Buff_t* const Buffer, const Ui_t* const Ui,
                         const Conf_t* const Config)
{
    idx_t chars_end_offset = CURRENT_LINE_LENGTH;

    // Previous lines. If scrolled. Only beginning is shown.
    for(idx_t line_idx = print__set_start_line(Buffer, Ui);
        line_idx < CURRENT_LINE_IDX; line_idx++)
    {
        print__another_line(Buffer, Ui, Config, line_idx);
    }

    // Display the last line without the linefeed.
    ui__print_line_number(Buffer, Config, CURRENT_LINE_IDX,
                          Ui->line_num_length);

    if(CURRENT_LINE_LENGTH < Ui->text_x)
    {
        if((CURRENT_LINE_LENGTH > 0)
        && (CURRENT_LINE[CURRENT_LINE_LENGTH - NUL_SZ] == '\n'))
        {
            chars_end_offset--;
        }
        print__line_with_tabs(Buffer, Config, CURRENT_LINE_IDX, 0,
                              chars_end_offset);

    }
    // Chars won't fits in the horizontal space.
    else if((CURRENT_LINE_LENGTH - Ui->text_x) >= Buffer->cursor_rev_x)
    {
        // Text will be scrolled. Not cursor.
        print__line_with_tabs(Buffer, Config, CURRENT_LINE_IDX,
                              CURSOR_X + CURSOR_SZ - Ui->text_x, CURSOR_X);
    }
    else
    {
        // Render only left part of the line. Cursor can scrolled.
        print__line_with_tabs(Buffer, Config, CURRENT_LINE_IDX, 0,
                              (idx_t) Ui->text_x - LF_SZ);
    }
}

void print__display_text(const Buff_t* const Buffer, const Ui_t* const Ui,
                         const Conf_t* const Config)
{
    if(Buffer->lines_idx < Ui->text_y)
    {
        print__fit_lines(Buffer, Ui, Config);
    }
    else if((CURRENT_LINE_IDX + IDX) < Ui->text_y)
    {
        print__shrink_lines(Buffer, Ui, Config);
    }
    else
    {
        // first_rendered_line_idx = ;
        print__scroll_lines(Buffer, Ui, Config);
    }
}
