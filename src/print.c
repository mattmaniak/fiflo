#include "print.h"

void print__line_with_tabs(const Buff_t* const Buffer,
                           const Conf_t* const Config,
                           const Syntax_t* const Syntax, const size_t ln_i,
                           const size_t start_ch_i, const size_t end_ch_i)
{
    size_t ch_i = start_ch_i;
    size_t ch_i_after_kwrd;
    char  ch;

    for(; ch_i < end_ch_i; ch_i++)
    {
        ch = Buffer->Lines[ln_i].txt[ch_i];
        pcard__print_at_offset(Config, ch_i);

        switch(ch) // Mostly whitespace highlighting.
        {
        case '\t':
            ui__colorize(Config->Color_whitespace.value);
            putchar(PRINT__TAB_HIGHLIGHT);
            break;

        case ' ':
            ui__colorize(Config->Color_whitespace.value);
            putchar(PRINT__SPACE_HIGHLIGHT);
            break;

        default: // Print words.
            ch_i_after_kwrd = syntax__paint_word(Syntax, Config,
                                                 &Buffer->Lines[ln_i],
                                                 end_ch_i, ch_i);
            if(ch_i == ch_i_after_kwrd)
            {
                ui__colorize(Config->Color_txt.value);
                putchar(ch);
            }
            else // Word printed and highlighted. Shift the index to the next.
            {
                ch_i = ch_i_after_kwrd;
            }
        }
        ui__colorize(0);
    }
}

size_t print__set_start_line(const Buff_t* const Buffer, const Ui_t* const Ui)
{
    size_t scrolled_lines = 0;

    if(BUFFER__ACTUAL_LN_I >= Ui->txtarea_h)
    {
        // Amount of lines to hide in a magic upper area.
        scrolled_lines = Buffer->ln_amount + SIZE__I - Ui->txtarea_h
                         - Buffer->cursor_rev_y;
    }
    return scrolled_lines;
}

void print__actual_line(const Buff_t* const Buffer, const Conf_t* const Config,
                        const Syntax_t* const Syntax, const Ui_t* const Ui)
{
    // There is a small amount of chars. Horizontal scroll isn't required.
    if(BUFFER__ACTUAL_LN.len < Ui->txtarea_w)
    {
        switch(BUFFER__ACTUAL_LN.len)
        {
        case 0:
            // Only the newline so print only the punch card.
            pcard__print_after_txt(Config, Ui, BUFFER__ACTUAL_LN.txt,
                                   BUFFER__ACTUAL_LN.len);
            break;

        default:
            if(BUFFER__ACTUAL_LN_I == Buffer->ln_amount)
            {
                print__line_with_tabs(Buffer, Config, Syntax,
                                      BUFFER__ACTUAL_LN_I, 0,
                                      BUFFER__ACTUAL_LN.len);
                pcard__print_after_txt(Config, Ui, BUFFER__ACTUAL_LN.txt,
                                       BUFFER__ACTUAL_LN.len);
            }
            else // Non-last line so ignore the LF and wrap after a punch card.
            {
                print__line_with_tabs(Buffer, Config, Syntax,
                                      BUFFER__ACTUAL_LN_I, 0,
                                      BUFFER__ACTUAL_LN.len - SIZE__LF);
                pcard__print_after_txt(Config, Ui, BUFFER__ACTUAL_LN.txt,
                                       BUFFER__ACTUAL_LN.len - SIZE__LF);
                WRAP_LN();
            }
        }
    }
    // Chars won't fit in a horizontal space.
    else if((BUFFER__ACTUAL_LN.len - Ui->txtarea_w)
            >= Buffer->cursor_rev_x)
    {
        // Render only a right part of a line.
        print__scroll_line_horizontally(Buffer, Config, Syntax, Ui);
    }
    else // Shrink the line.
    {
        print__line_with_tabs(Buffer, Config, Syntax, BUFFER__ACTUAL_LN_I,
                              0, (const size_t) Ui->txtarea_w - SIZE__LF);

        // Not last rendered line so wrap it.
        if(((BUFFER__ACTUAL_LN_I + SIZE__I) < Ui->txtarea_h)
           && (BUFFER__ACTUAL_LN_I != Buffer->ln_amount))
        {
            WRAP_LN();
        }
    }
}

void print__another_line(const Buff_t* const Buffer,
                         const Conf_t* const Config,
                         const Syntax_t* const Syntax, const Ui_t* const Ui,
                         const size_t ln_i)
{
    const size_t start_ch_i = (-Ui->pcard_delta_x < 0) ? 0
                               : (const size_t) -Ui->pcard_delta_x;

    ui__print_line_number(Buffer, Config, ln_i, Ui->ln_num_len);

    // Ignore the linefeed and print it after the wrap line.
    if(Buffer->Lines[ln_i].len < Ui->txtarea_w)
    {
        /* Using the -Ui->pcard_delta_x as the initiale offset scroll all other
           lines horizontally. */

        print__line_with_tabs(Buffer, Config, Syntax, ln_i, start_ch_i,
                              Buffer->Lines[ln_i].len - SIZE__LF);

        pcard__print_after_txt(Config, Ui, Buffer->Lines[ln_i].txt,
                               Buffer->Lines[ln_i].len - SIZE__LF);
        WRAP_LN();
    }
    else
    {
        print__line_with_tabs(Buffer, Config, Syntax, ln_i, start_ch_i,
                              (const size_t) Ui->txtarea_w - SIZE__LF);
        WRAP_LN();
    }
}

void print__scroll_line_horizontally(const Buff_t* const Buffer,
                                     const Conf_t* const Config,
                                     const Syntax_t* const Syntax,
                                     const Ui_t* const Ui)
{
    // AT text will be scrolled. Not the cursor.
    print__line_with_tabs(Buffer, Config, Syntax, BUFFER__ACTUAL_LN_I,
                          BUFFER__CURSOR_X + SIZE__CURSOR - Ui->txtarea_w,
                          BUFFER__CURSOR_X);

    /* Sometimes this is needed because the "window__fill" function renders the
       smallest required amount of linefeeds. In other cases the linefeed is
       provided by the char in a line. */
    if(BUFFER__CURSOR_Y_SCROLLED)
    {
        WRAP_LN();
    }
}

void print__fit_lines(const Buff_t* const Buffer, const Conf_t* const Config,
                      const Syntax_t* const Syntax, const Ui_t* const Ui)
{
    size_t ln_i = 0;

    for(; ln_i < BUFFER__ACTUAL_LN_I; ln_i++)
    {
        print__another_line(Buffer, Config, Syntax, Ui, ln_i);
    }
    ui__print_line_number(Buffer, Config, BUFFER__ACTUAL_LN_I,
                          Ui->ln_num_len);

    print__actual_line(Buffer, Config, Syntax, Ui);

    if(BUFFER__CURSOR_Y_SCROLLED)
    {
        for(ln_i += SIZE__LN; ln_i < Buffer->ln_amount; ln_i++)
        {
            print__another_line(Buffer, Config, Syntax, Ui, ln_i);
        }
        ui__print_line_number(Buffer, Config, Buffer->ln_amount,
                              Ui->ln_num_len);

        if(BUFFER__LAST_LN.len < Ui->txtarea_w)
        {
            print__line_with_tabs(Buffer, Config, Syntax, Buffer->ln_amount,
                                  0, BUFFER__LAST_LN.len);
        }
        else
        {
            print__line_with_tabs(Buffer, Config, Syntax, Buffer->ln_amount,
                                  0, (const size_t) Ui->txtarea_w - SIZE__LF);
        }
        pcard__print_after_txt(Config, Ui, BUFFER__LAST_LN.txt,
                               BUFFER__LAST_LN.len);
    }
}

void print__shrink_lines(const Buff_t* const Buffer,
                         const Conf_t* const Config,
                         const Syntax_t* const Syntax, const Ui_t* const Ui)
{
    const size_t last_ln_i = (const size_t) Ui->txtarea_h - SIZE__I;
    size_t       ln_i      = 0;

    // Previous lines. If they are scrolled. Only a beginning is shown.
    for(; ln_i < BUFFER__ACTUAL_LN_I; ln_i++)
    {
        print__another_line(Buffer, Config, Syntax, Ui, ln_i);
    }
    ui__print_line_number(Buffer, Config, BUFFER__ACTUAL_LN_I,
                          Ui->ln_num_len);

    print__actual_line(Buffer, Config, Syntax, Ui);

    // Next lines. If they are scrolled. Only a beginning is shown.
    for(ln_i = BUFFER__ACTUAL_LN_I + SIZE__I; ln_i < last_ln_i;
        ln_i++)
    {
        print__another_line(Buffer, Config, Syntax, Ui, ln_i);
    }
    ui__print_line_number(Buffer, Config, last_ln_i, Ui->ln_num_len);

    if(Buffer->Lines[last_ln_i].len < Ui->txtarea_w)
    {
        print__line_with_tabs(Buffer, Config, Syntax, last_ln_i, 0,
                              Buffer->Lines[last_ln_i].len - SIZE__LF);
    }
    else
    {
        print__line_with_tabs(Buffer, Config, Syntax, last_ln_i, 0,
                              (const size_t) Ui->txtarea_w - SIZE__LF);
    }
    pcard__print_after_txt(Config, Ui, Buffer->Lines[last_ln_i].txt,
                           Buffer->Lines[last_ln_i].len - SIZE__LF);
}

void print__scroll_lines(const Buff_t* const Buffer,
                         const Conf_t* const Config,
                         const Syntax_t* const Syntax, const Ui_t* const Ui)
{
    size_t end_ch_i   = BUFFER__ACTUAL_LN.len;
    size_t start_ln_i = print__set_start_line(Buffer, Ui);

    // Previous lines. If they are scrolled. Only a beginning is shown.
    for(size_t ln_i = start_ln_i; ln_i < BUFFER__ACTUAL_LN_I;
        ln_i++)
    {
        print__another_line(Buffer, Config, Syntax, Ui, ln_i);
    }

    // Display a last line without the linefeed to prevent a lbar breaking.
    ui__print_line_number(Buffer, Config, BUFFER__ACTUAL_LN_I,
                          Ui->ln_num_len);

    if(BUFFER__ACTUAL_LN.len < Ui->txtarea_w)
    {
        if((BUFFER__ACTUAL_LN.len > 0)
           && (BUFFER__ACTUAL_LN.txt[BUFFER__ACTUAL_LN.len - SIZE__NUL]
           == '\n'))
        {
            end_ch_i--;
        }
        print__line_with_tabs(Buffer, Config, Syntax, BUFFER__ACTUAL_LN_I,
                              0, end_ch_i);
    }
    // Chars won't fit in a horizontal space.
    else if((BUFFER__ACTUAL_LN.len - Ui->txtarea_w) >= Buffer->cursor_rev_x)
    {
        // Text will be scrolled. Not cursor.
        print__line_with_tabs(Buffer, Config, Syntax, BUFFER__ACTUAL_LN_I,
                              BUFFER__CURSOR_X + SIZE__CURSOR - Ui->txtarea_w,
                              BUFFER__CURSOR_X);
    }
    else
    {
        // Render only left part of a line. The cursor can be scrolled.
        print__line_with_tabs(Buffer, Config, Syntax, BUFFER__ACTUAL_LN_I,
                              0, (const size_t) Ui->txtarea_w - SIZE__LF);
    }
    pcard__print_after_txt(Config, Ui, BUFFER__ACTUAL_LN.txt,
                           (Buffer->cursor_rev_y == 0)
                           ? BUFFER__ACTUAL_LN.len
                           : BUFFER__ACTUAL_LN.len - SIZE__LF);
}

void print__display_text(const Buff_t* const Buffer,
                         const Conf_t* const Config,
                         const Syntax_t* const Syntax, const Ui_t* const Ui)
{
    if(Buffer->ln_amount < Ui->txtarea_h)
    {
        print__fit_lines(Buffer, Config, Syntax, Ui);
    }
    else if((BUFFER__ACTUAL_LN_I + SIZE__I) < Ui->txtarea_h)
    {
        print__shrink_lines(Buffer, Config, Syntax, Ui);
    }
    else
    {
        print__scroll_lines(Buffer, Config, Syntax, Ui);
    }
}
