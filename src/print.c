#include "print.h"

void print__line_with_tabs(const Buff_t* const Buffer,
                           const Conf_t* const Config,
                           const Syntax_t* const Syntax, const Ui_t* const Ui,
                           const idx_t ln_idx, const idx_t start_ch_idx,
                           const idx_t end_ch_idx)
{
    idx_t ch_idx = start_ch_idx;

    for(; ch_idx < end_ch_idx; ch_idx++)
    {
        const char ch = Buffer->Lines[ln_idx].txt[ch_idx];

        // Whitespace highlighting.
        if(ch == '\t')
        {
            ui__colorize(Config->Color_whitespace.value);
            if(ln_idx != BUFFER__ACTUAL_LINE_IDX)
            {
                if(ch_idx == (const idx_t) (Config->Punch_card_width.value - IDX + Ui->pcard_delta_x))
                {
                    ui__colorize(Config->Color_ui.value + 10);
                }
                putchar(PRINT__TAB_HIGHLIGHT);
                ui__colorize(0);
            }
            else
            {
                if(ch_idx == (const idx_t) (Config->Punch_card_width.value - IDX))
                {
                    ui__colorize(Config->Color_ui.value + 10);
                }
                putchar(PRINT__TAB_HIGHLIGHT);
            }
        }
        else if(ch == ' ')
        {
            ui__colorize(Config->Color_whitespace.value);
            // if(ch_idx == (const idx_t) (Config->Punch_card_width.value - IDX + Ui->pcard_delta_x))
            if(ch_idx == (const idx_t) (Config->Punch_card_width.value - IDX))
            {
                ui__colorize(Config->Color_ui.value + 10);
            }
            putchar(PRINT__SPACE_HIGHLIGHT);
            ui__colorize(0);
        }
        else // Print words.
        {
            const idx_t ch_idx_after_highlighting =
            syntax__paint_word(Syntax, Config, &Buffer->Lines[ln_idx], Ui,
                               end_ch_idx, ch_idx);

            if(ch_idx == ch_idx_after_highlighting)
            {
                ui__colorize(Config->Color_text.value);
                putchar(ch);
                ui__colorize(0);
            }
            else // Word printed and highlighted. Shift the index to the next.
            {
                ch_idx = ch_idx_after_highlighting;
            }
        }
        ui__colorize(0);
    }
}

void print__punch_card(const Conf_t* const Config, const Ui_t* const Ui,
                       const idx_t ln_len)
{
    if((Ui->textarea_w >= Config->Punch_card_width.value)
       && ((ln_len + IDX) < (const idx_t) Config->Punch_card_width.value))
    {
        if(-Ui->pcard_delta_x < (const int) (Config->Punch_card_width.value - ln_len - IDX))
        {
            printf("%*s", (const unsigned int)
                   Ui->pcard_delta_x + Config->Punch_card_width.value - ln_len - IDX, " ");

            ui__colorize(Config->Color_ui.value + 10);
            putchar(' ');
            ui__colorize(0);
        }
    }
}

idx_t print__set_start_line(const Buff_t* const Buffer, const Ui_t* const Ui)
{
    idx_t scrolled_lines = 0;

    if(BUFFER__ACTUAL_LINE_IDX >= Ui->textarea_h)
    {
        // Amount of lines to hide in a magic upper area.
        scrolled_lines = Buffer->lines_amount + IDX - Ui->textarea_h
                         - Buffer->cursor_rev_y;
    }
    return scrolled_lines;
}

void print__actual_line(const Buff_t* const Buffer, const Conf_t* const Config,
                        const Syntax_t* const Syntax, const Ui_t* const Ui)
{
    // There is a small amount of chars. Horizontal scroll isn't required.
    if(BUFFER__ACTUAL_LINE.len < Ui->textarea_w)
    {
        if(BUFFER__ACTUAL_LINE.len == 0)
        {
            // Only the newline so print only the punch card.
            print__punch_card(Config, Ui, BUFFER__ACTUAL_LINE.len);
        }
        else
        {
            // Last line so print a whole line.
            if(BUFFER__ACTUAL_LINE_IDX == Buffer->lines_amount)
            {
                print__line_with_tabs(Buffer, Config, Syntax, Ui,
                                      BUFFER__ACTUAL_LINE_IDX, 0,
                                      BUFFER__ACTUAL_LINE.len);
                print__punch_card(Config, Ui, BUFFER__ACTUAL_LINE.len);
            }
            else // Non-last line so ignore the LF and wrap after a punch card.
            {
                print__line_with_tabs(Buffer, Config, Syntax, Ui,
                                      BUFFER__ACTUAL_LINE_IDX, 0,
                                      BUFFER__ACTUAL_LINE.len - LF_SZ);
                print__punch_card(Config, Ui, BUFFER__ACTUAL_LINE.len - LF_SZ);
                WRAP_LINE();
            }
        }
    }
    // Chars won't fit in a horizontal space.
    else if((BUFFER__ACTUAL_LINE.len - Ui->textarea_w)
            >= Buffer->cursor_rev_x)
    {
        // Render only a right part of a line.
        print__scroll_line_horizontally(Buffer, Config, Syntax, Ui);
    }
    else // Shrink the line.
    {
        print__line_with_tabs(Buffer, Config, Syntax, Ui, BUFFER__ACTUAL_LINE_IDX,
                              0, (const idx_t) Ui->textarea_w - LF_SZ);

        // Not last rendered line so wrap it.
        if(((BUFFER__ACTUAL_LINE_IDX + IDX) < Ui->textarea_h)
           && (BUFFER__ACTUAL_LINE_IDX != Buffer->lines_amount))
        {
            WRAP_LINE();
        }
    }
}

void print__another_line(const Buff_t* const Buffer,
                         const Conf_t* const Config,
                         const Syntax_t* const Syntax, const Ui_t* const Ui,
                         const idx_t ln_idx)
{
    ui__print_line_number(Buffer, Config, ln_idx, Ui->line_num_len);

    // Ignore the linefeed and print it after the wrap line.
    if(Buffer->Lines[ln_idx].len < Ui->textarea_w)
    {
        print__line_with_tabs(Buffer, Config, Syntax, Ui, ln_idx, 0,
                              Buffer->Lines[ln_idx].len - LF_SZ);
        print__punch_card(Config, Ui, Buffer->Lines[ln_idx].len - LF_SZ);
        WRAP_LINE();
    }
    else
    {
        print__line_with_tabs(Buffer, Config, Syntax, Ui, ln_idx, 0,
                              (const idx_t) Ui->textarea_w - LF_SZ);
        WRAP_LINE();
    }
}

void print__scroll_line_horizontally(const Buff_t* const Buffer,
                                     const Conf_t* const Config,
                                     const Syntax_t* const Syntax,
                                     const Ui_t* const Ui)
{
    // AT text will be scrolled. Not the cursor.
    print__line_with_tabs(Buffer, Config, Syntax, Ui, BUFFER__ACTUAL_LINE_IDX,
                          BUFFER__CURSOR_X + CURSOR_SZ - Ui->textarea_w,
                          BUFFER__CURSOR_X);

    /* Sometimes this is needed because the "window__fill" function renders the
       smallest required amount of linefeeds. In other cases the linefeed is
       provided by the char in a line. */
    if(BUFFER__CURSOR_Y_SCROLLED)
    {
        WRAP_LINE();
    }
}

void print__fit_lines(const Buff_t* const Buffer, const Conf_t* const Config,
                      const Syntax_t* const Syntax, const Ui_t* const Ui)
{
    const idx_t actual_line_sz = 1;
    idx_t       ln_idx;

    for(ln_idx = 0; ln_idx < BUFFER__ACTUAL_LINE_IDX; ln_idx++)
    {
        print__another_line(Buffer, Config, Syntax, Ui, ln_idx);
    }
    ui__print_line_number(Buffer, Config, BUFFER__ACTUAL_LINE_IDX,
                          Ui->line_num_len);

    print__actual_line(Buffer, Config, Syntax, Ui);

    if(BUFFER__CURSOR_Y_SCROLLED)
    {
        for(ln_idx += actual_line_sz; ln_idx < Buffer->lines_amount; ln_idx++)
        {
            print__another_line(Buffer, Config, Syntax, Ui, ln_idx);
        }
        ui__print_line_number(Buffer, Config, Buffer->lines_amount,
                              Ui->line_num_len);

        if(BUFFER__LAST_LINE.len < Ui->textarea_w)
        {
            print__line_with_tabs(Buffer, Config, Syntax, Ui,
                                  Buffer->lines_amount, 0,
                                  BUFFER__LAST_LINE.len);
        }
        else
        {
            print__line_with_tabs(Buffer, Config, Syntax, Ui,
                                  Buffer->lines_amount, 0,
                                  (const idx_t) Ui->textarea_w - LF_SZ);
        }
        print__punch_card(Config, Ui, BUFFER__LAST_LINE.len);
    }
}

void print__shrink_lines(const Buff_t* const Buffer,
                         const Conf_t* const Config,
                         const Syntax_t* const Syntax, const Ui_t* const Ui)
{
    const idx_t last_ln_idx = (const idx_t) Ui->textarea_h - IDX;
    idx_t       ln_idx      = 0;

    // Previous lines. If they are scrolled. Only a beginning is shown.
    for(; ln_idx < BUFFER__ACTUAL_LINE_IDX; ln_idx++)
    {
        print__another_line(Buffer, Config, Syntax, Ui, ln_idx);
    }
    ui__print_line_number(Buffer, Config, BUFFER__ACTUAL_LINE_IDX,
                          Ui->line_num_len);

    print__actual_line(Buffer, Config, Syntax, Ui);

    // Next lines. If they are scrolled. Only a beginning is shown.
    for(ln_idx = BUFFER__ACTUAL_LINE_IDX + IDX; ln_idx < last_ln_idx; ln_idx++)
    {
        print__another_line(Buffer, Config, Syntax, Ui, ln_idx);
    }
    ui__print_line_number(Buffer, Config, last_ln_idx, Ui->line_num_len);

    if(Buffer->Lines[last_ln_idx].len < Ui->textarea_w)
    {
        print__line_with_tabs(Buffer, Config, Syntax, Ui, last_ln_idx, 0,
                              Buffer->Lines[last_ln_idx].len - LF_SZ);
    }
    else
    {
        print__line_with_tabs(Buffer, Config, Syntax, Ui, last_ln_idx, 0,
                              (const idx_t) Ui->textarea_w - LF_SZ);
    }
    print__punch_card(Config, Ui, Buffer->Lines[last_ln_idx].len - LF_SZ);
}

void print__scroll_lines(const Buff_t* const Buffer,
                         const Conf_t* const Config,
                         const Syntax_t* const Syntax, const Ui_t* const Ui)
{
    idx_t end_ch_idx = BUFFER__ACTUAL_LINE.len;

    // Previous lines. If they are scrolled. Only a beginning is shown.
    for(idx_t ln_idx = print__set_start_line(Buffer, Ui);
        ln_idx < BUFFER__ACTUAL_LINE_IDX; ln_idx++)
    {
        print__another_line(Buffer, Config, Syntax, Ui, ln_idx);
    }

    // Display a last line without the linefeed to prevent a lbar breaking.
    ui__print_line_number(Buffer, Config, BUFFER__ACTUAL_LINE_IDX,
                          Ui->line_num_len);

    if(BUFFER__ACTUAL_LINE.len < Ui->textarea_w)
    {
        if((BUFFER__ACTUAL_LINE.len > 0)
           && (BUFFER__ACTUAL_LINE.txt[BUFFER__ACTUAL_LINE.len - NUL_SZ]
           == '\n'))
        {
            end_ch_idx--;
        }
        print__line_with_tabs(Buffer, Config, Syntax, Ui,
                              BUFFER__ACTUAL_LINE_IDX, 0, end_ch_idx);

    }
    // Chars won't fit in a horizontal space.
    else if((BUFFER__ACTUAL_LINE.len - Ui->textarea_w)
            >= Buffer->cursor_rev_x)
    {
        // Text will be scrolled. Not cursor.
        print__line_with_tabs(Buffer, Config, Syntax, Ui,
                              BUFFER__ACTUAL_LINE_IDX,
                              BUFFER__CURSOR_X + CURSOR_SZ - Ui->textarea_w,
                              BUFFER__CURSOR_X);
    }
    else
    {
        // Render only left part of a line. The cursor can be scrolled.
        print__line_with_tabs(Buffer, Config, Syntax, Ui,
                              BUFFER__ACTUAL_LINE_IDX, 0,
                              (const idx_t) Ui->textarea_w - LF_SZ);
    }
    print__punch_card(Config, Ui, (Buffer->cursor_rev_y == 0)
                      ? BUFFER__ACTUAL_LINE.len
                      : BUFFER__ACTUAL_LINE.len - LF_SZ);
}

void print__display_text(const Buff_t* const Buffer,
                         const Conf_t* const Config,
                         const Syntax_t* const Syntax, const Ui_t* const Ui)
{
    if(Buffer->lines_amount < Ui->textarea_h)
    {
        print__fit_lines(Buffer, Config, Syntax, Ui);
    }
    else if((BUFFER__ACTUAL_LINE_IDX + IDX) < Ui->textarea_h)
    {
        print__shrink_lines(Buffer, Config, Syntax, Ui);
    }
    else
    {
        // first_rendered_ln_idx = ;
        print__scroll_lines(Buffer, Config, Syntax, Ui);
    }
}
