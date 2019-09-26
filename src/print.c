#include "print.h"

void print__line_with_tabs(const V_file_t* const V_file,
                           const Conf_t* const Config,
                           const Syntax_t* const Syntax, const size_t ln_i,
                           const size_t start_ch_i, const size_t end_ch_i)
{
    size_t ch_i = start_ch_i;
    size_t ch_i_after_kwrd;
    char   ch;

    for(; ch_i < end_ch_i; ch_i++)
    {
        ch = V_file->Lines[ln_i].txt[ch_i];
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
                                                 &V_file->Lines[ln_i],
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

size_t print__set_start_line(const V_file_t* const V_file,
                             const Ui_t* const Ui)
{
    size_t scrolled_lines = 0;

    if(V_FILE__ACTUAL_LN_I >= Ui->txtarea_h)
    {
        // Amount of lines to hide in a magic upper area.
        scrolled_lines = V_file->ln_amount + SIZE__I - Ui->txtarea_h
                         - V_file->cursor_rev_y;
    }
    return scrolled_lines;
}

void print__actual_line(const V_file_t* const V_file,
                        const Conf_t* const Config,
                        const Syntax_t* const Syntax, const Ui_t* const Ui)
{
    // There is a small amount of chars. Horizontal scroll isn't required.
    if(V_FILE__ACTUAL_LN.len < Ui->txtarea_w)
    {
        switch(V_FILE__ACTUAL_LN.len)
        {
        case 0:
            // Only the newline so print only the punch card.
            pcard__print_after_txt(Config, Ui, V_FILE__ACTUAL_LN.txt,
                                   V_FILE__ACTUAL_LN.len);
            break;

        default:
            if(V_FILE__ACTUAL_LN_I == V_file->ln_amount)
            {
                print__line_with_tabs(V_file, Config, Syntax,
                                      V_FILE__ACTUAL_LN_I, 0,
                                      V_FILE__ACTUAL_LN.len);

                pcard__print_after_txt(Config, Ui, V_FILE__ACTUAL_LN.txt,
                                       V_FILE__ACTUAL_LN.len);
            }
            else // Non-last line so ignore the LF and wrap after a punch card.
            {
                print__line_with_tabs(V_file, Config, Syntax,
                                      V_FILE__ACTUAL_LN_I, 0,
                                      V_FILE__ACTUAL_LN.len - SIZE__LF);

                pcard__print_after_txt(Config, Ui, V_FILE__ACTUAL_LN.txt,
                                       V_FILE__ACTUAL_LN.len - SIZE__LF);
                UI__WRAP_LN();
            }
        }
    }
    // Chars won't fit in a horizontal space.
    else if((V_FILE__ACTUAL_LN.len - Ui->txtarea_w) >= V_file->cursor_rev_x)
    {
        // Render only a right part of a line.
        print__scroll_line_horizontally(V_file, Config, Syntax, Ui);
    }
    else // Shrink the line.
    {
        print__line_with_tabs(V_file, Config, Syntax, V_FILE__ACTUAL_LN_I,
                              0, (const size_t) Ui->txtarea_w - SIZE__LF);

        // Not last rendered line so wrap it.
        if(((V_FILE__ACTUAL_LN_I + SIZE__I) < Ui->txtarea_h)
           && (V_FILE__ACTUAL_LN_I != V_file->ln_amount))
        {
            UI__WRAP_LN();
        }
    }
}

void print__another_line(const V_file_t* const V_file,
                         const Conf_t* const Config,
                         const Syntax_t* const Syntax, const Ui_t* const Ui,
                         const size_t ln_i)
{
    const size_t start_ch_i = (-Ui->pcard_delta_x < 0) ? 0
                              : (size_t) -Ui->pcard_delta_x;

    ui__print_line_number(V_file, Config, ln_i, Ui->ln_num_len);

    // Ignore the linefeed and print it after the wrap line.
    if(V_file->Lines[ln_i].len < Ui->txtarea_w)
    {
        /* Using the -Ui->pcard_delta_x as the initiale offset scroll all other
           lines horizontally. */

        print__line_with_tabs(V_file, Config, Syntax, ln_i, start_ch_i,
                              V_file->Lines[ln_i].len - SIZE__LF);

        pcard__print_after_txt(Config, Ui, V_file->Lines[ln_i].txt,
                               V_file->Lines[ln_i].len - SIZE__LF);
        UI__WRAP_LN();
    }
    else
    {
        print__line_with_tabs(V_file, Config, Syntax, ln_i, start_ch_i,
                              (const size_t) Ui->txtarea_w - SIZE__LF);
        UI__WRAP_LN();
    }
}

void print__scroll_line_horizontally(const V_file_t* const V_file,
                                     const Conf_t* const Config,
                                     const Syntax_t* const Syntax,
                                     const Ui_t* const Ui)
{
    // AT text will be scrolled. Not the cursor.
    print__line_with_tabs(V_file, Config, Syntax, V_FILE__ACTUAL_LN_I,
                          V_FILE__CURSOR_X + SIZE__CURSOR - Ui->txtarea_w,
                          V_FILE__CURSOR_X);

    /* Sometimes this is needed because the "window__fill" function renders the
       smallest required amount of linefeeds. In other cases the linefeed is
       provided by the char in a line. */
    if(V_FILE__CURSOR_Y_SCROLLED)
    {
        UI__WRAP_LN();
    }
}

void print__fit_lines(const V_file_t* const V_file, const Conf_t* const Config,
                      const Syntax_t* const Syntax, const Ui_t* const Ui)
{
    size_t ln_i = 0;

    for(; ln_i < V_FILE__ACTUAL_LN_I; ln_i++)
    {
        print__another_line(V_file, Config, Syntax, Ui, ln_i);
    }
    ui__print_line_number(V_file, Config, V_FILE__ACTUAL_LN_I,
                          Ui->ln_num_len);

    print__actual_line(V_file, Config, Syntax, Ui);

    if(V_FILE__CURSOR_Y_SCROLLED)
    {
        for(ln_i += SIZE__LN; ln_i < V_file->ln_amount; ln_i++)
        {
            print__another_line(V_file, Config, Syntax, Ui, ln_i);
        }
        ui__print_line_number(V_file, Config, V_file->ln_amount,
                              Ui->ln_num_len);

        if(V_FILE__LAST_LN.len < Ui->txtarea_w)
        {
            print__line_with_tabs(V_file, Config, Syntax, V_file->ln_amount,
                                  0, V_FILE__LAST_LN.len);
        }
        else
        {
            print__line_with_tabs(V_file, Config, Syntax, V_file->ln_amount,
                                  0, (const size_t) Ui->txtarea_w - SIZE__LF);
        }
        pcard__print_after_txt(Config, Ui, V_FILE__LAST_LN.txt,
                               V_FILE__LAST_LN.len);
    }
}

void print__shrink_lines(const V_file_t* const V_file,
                         const Conf_t* const Config,
                         const Syntax_t* const Syntax, const Ui_t* const Ui)
{
    const size_t last_ln_i = (size_t) Ui->txtarea_h - SIZE__I;
    size_t       ln_i      = 0;

    // Previous lines. If they are scrolled. Only a beginning is shown.
    for(; ln_i < V_FILE__ACTUAL_LN_I; ln_i++)
    {
        print__another_line(V_file, Config, Syntax, Ui, ln_i);
    }
    ui__print_line_number(V_file, Config, V_FILE__ACTUAL_LN_I, Ui->ln_num_len);
    print__actual_line(V_file, Config, Syntax, Ui);

    // Next lines. If they are scrolled. Only a beginning is shown.
    for(ln_i = V_FILE__ACTUAL_LN_I + SIZE__I; ln_i < last_ln_i; ln_i++)
    {
        print__another_line(V_file, Config, Syntax, Ui, ln_i);
    }
    ui__print_line_number(V_file, Config, last_ln_i, Ui->ln_num_len);

    if(V_file->Lines[last_ln_i].len < Ui->txtarea_w)
    {
        print__line_with_tabs(V_file, Config, Syntax, last_ln_i, 0,
                              V_file->Lines[last_ln_i].len - SIZE__LF);
    }
    else
    {
        print__line_with_tabs(V_file, Config, Syntax, last_ln_i, 0,
                              (size_t) Ui->txtarea_w - SIZE__LF);
    }
    pcard__print_after_txt(Config, Ui, V_file->Lines[last_ln_i].txt,
                           V_file->Lines[last_ln_i].len - SIZE__LF);
}

void print__scroll_lines(const V_file_t* const V_file,
                         const Conf_t* const Config,
                         const Syntax_t* const Syntax, const Ui_t* const Ui)
{
    size_t end_ch_i   = V_FILE__ACTUAL_LN.len;
    size_t start_ln_i = print__set_start_line(V_file, Ui);

    // Previous lines. If they are scrolled. Only a beginning is shown.
    for(size_t ln_i = start_ln_i; ln_i < V_FILE__ACTUAL_LN_I; ln_i++)
    {
        print__another_line(V_file, Config, Syntax, Ui, ln_i);
    }

    // Display a last line without the linefeed to prevent a lbar breaking.
    ui__print_line_number(V_file, Config, V_FILE__ACTUAL_LN_I, Ui->ln_num_len);

    if(V_FILE__ACTUAL_LN.len < Ui->txtarea_w)
    {
        if((V_FILE__ACTUAL_LN.len > 0)
           && (V_FILE__ACTUAL_LN.txt[V_FILE__ACTUAL_LN.len - SIZE__NUL]
               == '\n'))
        {
            end_ch_i--;
        }
        print__line_with_tabs(V_file, Config, Syntax, V_FILE__ACTUAL_LN_I, 0,
                              end_ch_i);
    }
    // Chars won't fit in a horizontal space.
    else if((V_FILE__ACTUAL_LN.len - Ui->txtarea_w) >= V_file->cursor_rev_x)
    {
        // Text will be scrolled. Not cursor.
        print__line_with_tabs(V_file, Config, Syntax, V_FILE__ACTUAL_LN_I,
                              V_FILE__CURSOR_X + SIZE__CURSOR - Ui->txtarea_w,
                              V_FILE__CURSOR_X);
    }
    else
    {
        // Render only left part of a line. The cursor can be scrolled.
        print__line_with_tabs(V_file, Config, Syntax, V_FILE__ACTUAL_LN_I,
                              0, (size_t) Ui->txtarea_w - SIZE__LF);
    }
    pcard__print_after_txt(Config, Ui, V_FILE__ACTUAL_LN.txt,
                           (V_file->cursor_rev_y == 0) ? V_FILE__ACTUAL_LN.len
                           : V_FILE__ACTUAL_LN.len - SIZE__LF);
}

void print__display_text(const V_file_t* const V_file,
                         const Conf_t* const Config,
                         const Syntax_t* const Syntax, const Ui_t* const Ui)
{
    if(V_file->ln_amount < Ui->txtarea_h)
    {
        print__fit_lines(V_file, Config, Syntax, Ui);
    }
    else if((V_FILE__ACTUAL_LN_I + SIZE__I) < Ui->txtarea_h)
    {
        print__shrink_lines(V_file, Config, Syntax, Ui);
    }
    else
    {
        print__scroll_lines(V_file, Config, Syntax, Ui);
    }
}
