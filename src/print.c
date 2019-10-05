#include "print.h"

void print__line_with_tabs(const V_file* const v_file,
                           const Config* const config,
                           const Syntax* const syntax, const size_t line_i,
                           const size_t start_char_i, const size_t end_char_i)
{
    size_t char_i_after_kwrd;
    char   ch;

    for(size_t char_i = start_char_i; char_i < end_char_i; char_i++)
    {
        ch = v_file->Lines[line_i].txt[char_i];
        pcard__print_at_offset(config, char_i);

        switch(ch) // Mostly whitespace highlighting.
        {
        case '\t':
        case ' ':
            ui__colorize(config->Color_whitespace.value);
            putchar((ch == '\t') ? PRINT__TAB_HIGHLIGHT
                    : PRINT__SPACE_HIGHLIGHT);
            break;

        default: // Print words.
            char_i_after_kwrd = syntax__paint_word(syntax, config,
                                                 &v_file->Lines[line_i],
                                                 end_char_i, char_i);
            if(char_i == char_i_after_kwrd)
            {
                ui__colorize(config->Color_txt.value);
                putchar(ch);
            }
            else // Word printed and highlighted. Shift the index to the next.
            {
                char_i = char_i_after_kwrd;
            }
        }
        ui__colorize(0);
    }
}

size_t print__set_start_line(const V_file* const v_file,
                             const Ui* const ui)
{
    if(V_FILE__ACTUAL_LINE_I >= ui->txtarea_h)
    {
        // Amount of lines to hide in a magic upper area.
        return v_file->lines_amount + SIZE__I - ui->txtarea_h
               - v_file->mirrored_cursor_y;
    }
    return 0;
}

void print__actual_line(const V_file* const v_file,
                        const Config* const config,
                        const Syntax* const syntax, const Ui* const ui)
{
    // There is a small amount of chars. Horizontal scroll isn't required.
    if(V_FILE__ACTUAL_LINE.len < ui->txtarea_w)
    {
        switch(V_FILE__ACTUAL_LINE.len)
        {
        case 0:
            // Only the newline so print only the punch card.
            pcard__print_after_txt(config, ui, V_FILE__ACTUAL_LINE.txt,
                                   V_FILE__ACTUAL_LINE.len);
            break;

        default:
            if(V_FILE__ACTUAL_LINE_I == v_file->lines_amount)
            {
                print__line_with_tabs(v_file, config, syntax,
                                      V_FILE__ACTUAL_LINE_I, 0,
                                      V_FILE__ACTUAL_LINE.len);

                pcard__print_after_txt(config, ui, V_FILE__ACTUAL_LINE.txt,
                                       V_FILE__ACTUAL_LINE.len);
            }
            else // Non-last line so ignore the LF and wrap after a punch card.
            {
                print__line_with_tabs(v_file, config, syntax,
                                      V_FILE__ACTUAL_LINE_I, 0,
                                      V_FILE__ACTUAL_LINE.len - SIZE__LF);

                pcard__print_after_txt(config, ui, V_FILE__ACTUAL_LINE.txt,
                                       V_FILE__ACTUAL_LINE.len - SIZE__LF);
                UI__WRAP_LINE();
            }
        }
    }
    // Chars won't fit in a horizontal space.
    else if((V_FILE__ACTUAL_LINE.len - ui->txtarea_w) >= v_file->mirrored_cursor_x)
    {
        // Render only a right part of a line.
        print__scroll_line_horizontally(v_file, config, syntax, ui);
    }
    else // Shrink the line.
    {
        print__line_with_tabs(v_file, config, syntax, V_FILE__ACTUAL_LINE_I, 0,
                              (size_t) ui->txtarea_w - SIZE__LF);

        // Not last rendered line so wrap it.
        if(((V_FILE__ACTUAL_LINE_I + SIZE__I) < ui->txtarea_h)
           && (V_FILE__ACTUAL_LINE_I != v_file->lines_amount))
        {
            UI__WRAP_LINE();
        }
    }
}

void print__another_line(const V_file* const v_file,
                         const Config* const config,
                         const Syntax* const syntax, const Ui* const ui,
                         const size_t line_i)
{
    const size_t start_char_i = (-ui->pcard_delta_x < 0) ? 0
                                : (size_t) -ui->pcard_delta_x;

    ui__print_line_number(v_file, config, line_i, ui->line_num_len);

    // Ignore the linefeed and print it after the wrap line.
    if(v_file->Lines[line_i].len < ui->txtarea_w)
    {
        /* Using the -ui->pcard_delta_x as the initiale offset scroll all other
           lines horizontally. */
        print__line_with_tabs(v_file, config, syntax, line_i, start_char_i,
                              v_file->Lines[line_i].len - SIZE__LF);

        pcard__print_after_txt(config, ui, v_file->Lines[line_i].txt,
                               v_file->Lines[line_i].len - SIZE__LF);
        UI__WRAP_LINE();
    }
    else
    {
        print__line_with_tabs(v_file, config, syntax, line_i, start_char_i,
                              (size_t) ui->txtarea_w - SIZE__LF);
        UI__WRAP_LINE();
    }
}

void print__scroll_line_horizontally(const V_file* const v_file,
                                     const Config* const config,
                                     const Syntax* const syntax,
                                     const Ui* const ui)
{
    // AT text will be scrolled. Not the cursor.
    print__line_with_tabs(v_file, config, syntax, V_FILE__ACTUAL_LINE_I,
                          V_FILE__CURSOR_X + SIZE__CURSOR - ui->txtarea_w,
                          V_FILE__CURSOR_X);

    /* Sometimes this is needed because the "window__fill" function renders the
       smallest required amount of linefeeds. In other cases the linefeed is
       provided by the char in a line. */
    if(V_FILE__CURSOR_Y_SCROLLED)
    {
        UI__WRAP_LINE();
    }
}

void print__fit_lines(const V_file* const v_file, const Config* const config,
                      const Syntax* const syntax, const Ui* const ui)
{
    size_t line_i = 0;

    for(; line_i < V_FILE__ACTUAL_LINE_I; line_i++)
    {
        print__another_line(v_file, config, syntax, ui, line_i);
    }
    ui__print_line_number(v_file, config, V_FILE__ACTUAL_LINE_I,
                          ui->line_num_len);

    print__actual_line(v_file, config, syntax, ui);

    if(V_FILE__CURSOR_Y_SCROLLED)
    {
        for(line_i += SIZE__LINE; line_i < v_file->lines_amount; line_i++)
        {
            print__another_line(v_file, config, syntax, ui, line_i);
        }
        ui__print_line_number(v_file, config, v_file->lines_amount,
                              ui->line_num_len);

        if(V_FILE__LAST_LINE.len < ui->txtarea_w)
        {
            print__line_with_tabs(v_file, config, syntax, v_file->lines_amount,
                                  0, V_FILE__LAST_LINE.len);
        }
        else
        {
            print__line_with_tabs(v_file, config, syntax, v_file->lines_amount,
                                  0, (size_t) ui->txtarea_w - SIZE__LF);
        }
        pcard__print_after_txt(config, ui, V_FILE__LAST_LINE.txt,
                               V_FILE__LAST_LINE.len);
    }
}

void print__shrink_lines(const V_file* const v_file,
                         const Config* const config,
                         const Syntax* const syntax, const Ui* const ui)
{
    const size_t start_char_i = (-ui->pcard_delta_x < 0) ? 0
                                : (size_t) -ui->pcard_delta_x;
    const size_t last_line_i  = (size_t) ui->txtarea_h - SIZE__I;
    size_t       line_i       = 0;

    // Previous lines. If they are scrolled. Only a beginning is shown.
    for(; line_i < V_FILE__ACTUAL_LINE_I; line_i++)
    {
        print__another_line(v_file, config, syntax, ui, line_i);
    }
    ui__print_line_number(v_file, config, V_FILE__ACTUAL_LINE_I, ui->line_num_len);
    print__actual_line(v_file, config, syntax, ui);

    // Next lines. If they are scrolled. Only a beginning is shown.
    for(line_i = V_FILE__ACTUAL_LINE_I + SIZE__I; line_i < last_line_i; line_i++)
    {
        print__another_line(v_file, config, syntax, ui, line_i);
    }
    ui__print_line_number(v_file, config, last_line_i, ui->line_num_len);

    if(v_file->Lines[last_line_i].len < ui->txtarea_w)
    {
        print__line_with_tabs(v_file, config, syntax, last_line_i, start_char_i,
                              v_file->Lines[last_line_i].len - SIZE__LF);
    }
    else
    {
        print__line_with_tabs(v_file, config, syntax, last_line_i, start_char_i,
                              (size_t) ui->txtarea_w - SIZE__LF);
    }
    pcard__print_after_txt(config, ui, v_file->Lines[last_line_i].txt,
                           v_file->Lines[last_line_i].len - SIZE__LF);
}

void print__scroll_lines(const V_file* const v_file,
                         const Config* const config,
                         const Syntax* const syntax, const Ui* const ui)
{
    size_t end_char_i   = V_FILE__ACTUAL_LINE.len;
    size_t start_line_i = print__set_start_line(v_file, ui);

    // Previous lines. If they are scrolled. Only a beginning is shown.
    for(size_t line_i = start_line_i; line_i < V_FILE__ACTUAL_LINE_I; line_i++)
    {
        print__another_line(v_file, config, syntax, ui, line_i);
    }

    // Display a last line without the linefeed to prevent a lbar breaking.
    ui__print_line_number(v_file, config, V_FILE__ACTUAL_LINE_I, ui->line_num_len);

    if(V_FILE__ACTUAL_LINE.len < ui->txtarea_w)
    {
        if((V_FILE__ACTUAL_LINE.len > 0)
           && (V_FILE__ACTUAL_LINE.txt[V_FILE__ACTUAL_LINE.len - SIZE__NUL]
               == '\n'))
        {
            end_char_i--;
        }
        print__line_with_tabs(v_file, config, syntax, V_FILE__ACTUAL_LINE_I, 0,
                              end_char_i);
    }
    // Chars won't fit in a horizontal space.
    else if((V_FILE__ACTUAL_LINE.len - ui->txtarea_w) >= v_file->mirrored_cursor_x)
    {
        // Text will be scrolled. Not cursor.
        print__line_with_tabs(v_file, config, syntax, V_FILE__ACTUAL_LINE_I,
                              V_FILE__CURSOR_X + SIZE__CURSOR - ui->txtarea_w,
                              V_FILE__CURSOR_X);
    }
    else
    {
        // Render only left part of a line. The cursor can be scrolled.
        print__line_with_tabs(v_file, config, syntax, V_FILE__ACTUAL_LINE_I, 0,
                              (size_t) ui->txtarea_w - SIZE__LF);
    }
    if(config->Pcard_w.value < ui->txtarea_w)
    {
        pcard__print_after_txt(config, ui, V_FILE__ACTUAL_LINE.txt,
                               (v_file->mirrored_cursor_y == 0) ? V_FILE__ACTUAL_LINE.len
                               : V_FILE__ACTUAL_LINE.len - SIZE__LF);
    }
}

void print__display_text(const V_file* const v_file,
                         const Config* const config,
                         const Syntax* const syntax, const Ui* const ui)
{
    if(v_file->lines_amount < ui->txtarea_h)
    {
        print__fit_lines(v_file, config, syntax, ui);
    }
    else if((V_FILE__ACTUAL_LINE_I + SIZE__I) < ui->txtarea_h)
    {
        print__shrink_lines(v_file, config, syntax, ui);
    }
    else
    {
        print__scroll_lines(v_file, config, syntax, ui);
    }
}
