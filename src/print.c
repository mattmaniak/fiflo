#include "print.h"

void print__line_with_tabs(const V_file* const v_file,
                           const Config* const config,
                           const Syntax* const syntax, const size_t line_i,
                           const size_t start_char_i, const size_t end_char_i)
{
    size_t char_i_after_keyword;
    char   ch;

    for(size_t char_i = start_char_i; char_i < end_char_i; char_i++)
    {
        ch = v_file->lines[line_i].txt[char_i];
        punched_card__print_at_offset(config, char_i);

        switch(ch) // Mostly whitespace highlighting.
        {
        case '\t':
        case ' ':
            ui__colorize(config->Color_whitespace.value);
            putchar((ch == '\t') ? PRINT__TAB_HIGHLIGHT
                    : PRINT__SPACE_HIGHLIGHT);
            break;

        default: // Print words.
            char_i_after_keyword = syntax__paint_word(syntax, config,
                                                      &v_file->lines[line_i],
                                                      end_char_i, char_i);
            if(char_i == char_i_after_keyword)
            {
                ui__colorize(config->Color_txt.value);
                putchar(ch);
            }
            else // Word printed and highlighted. Shift the index to the next.
            {
                char_i = char_i_after_keyword;
            }
        }
        ui__colorize(0);
    }
}

size_t print__set_start_line(const V_file* const v_file,
                             const Ui* const ui)
{
    if(v_file_cursor_y(v_file) >= ui->txtarea_h)
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
    if(v_file_actual_line(v_file)->length < ui->txtarea_w)
    {
        if(v_file_cursor_y(v_file) == v_file->lines_amount)
        {
            print__line_with_tabs(v_file, config, syntax,
                                  v_file_cursor_y(v_file), 0,
                                  v_file_actual_line(v_file)->length);
        }
        else // Non-last line so ignore the LF and wrap after a punch card.
        {
            print__line_with_tabs(v_file, config, syntax,
                                  v_file_cursor_y(v_file), 0,
                                  v_file_actual_line(v_file)->length
                                  - SIZE__LF);
            UI__WRAP_LINE();
        }
    }
    // Chars won't fit in a horizontal space.
    else if((v_file_actual_line(v_file)->length - ui->txtarea_w)
            >= v_file->mirrored_cursor_x)
    {
        // Render only a right part of a line.
        print__scroll_line_horizontally(v_file, config, syntax, ui);
    }
    else // Shrink the line.
    {
        print__line_with_tabs(v_file, config, syntax, v_file_cursor_y(v_file),
                              0, (size_t) ui->txtarea_w - SIZE__LF);

        // Not last rendered line so wrap it.
        if(((v_file_cursor_y(v_file) + SIZE__I) < ui->txtarea_h)
           && (v_file_cursor_y(v_file) != v_file->lines_amount))
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
    const size_t start_char_i = (-ui->punched_card_delta_x < 0) ? 0
                                : (size_t) -ui->punched_card_delta_x;

    size_t end_char_i = (size_t) (ui->txtarea_w - ui->punched_card_delta_x
                                  - SIZE__LF);

    ui__print_line_number(v_file, config, line_i, ui->line_number_length);

    // Ignore the linefeed and print it after the wrap line.
    if(v_file->lines[line_i].length < ui->txtarea_w)
    {
        /* Using the -ui->punched_card_delta_x as the initiale offset scroll
           all other lines horizontally. */
        print__line_with_tabs(v_file, config, syntax, line_i, start_char_i,
                              v_file->lines[line_i].length - SIZE__LF);
        UI__WRAP_LINE();
    }
    else
    {
        if(end_char_i >= v_file->lines[line_i].length)
        {
            end_char_i = v_file->lines[line_i].length - SIZE__LF;
        }
        print__line_with_tabs(v_file, config, syntax, line_i, start_char_i,
                              end_char_i);
        UI__WRAP_LINE();
    }
}

void print__scroll_line_horizontally(const V_file* const v_file,
                                     const Config* const config,
                                     const Syntax* const syntax,
                                     const Ui* const ui)
{
    // Text will be scrolled. Not the cursor.
    print__line_with_tabs(v_file, config, syntax, v_file_cursor_y(v_file),
                          v_file_cursor_x(v_file) + SIZE__CURSOR
                          - ui->txtarea_w,
                          v_file_cursor_x(v_file));

    /* Sometimes this is needed because the "window__fill" function renders the
       smallest required amount of linefeeds. In other cases the linefeed is
       provided by the char in a line. */
    if(v_file__is_cursor_y_scrolled(v_file))
    {
        UI__WRAP_LINE();
    }
}

void print__fit_lines(const V_file* const v_file, const Config* const config,
                      const Syntax* const syntax, const Ui* const ui)
{
    size_t line_i = 0;

    for(; line_i < v_file_cursor_y(v_file); line_i++)
    {
        print__another_line(v_file, config, syntax, ui, line_i);
    }
    ui__print_line_number(v_file, config, v_file_cursor_y(v_file),
                          ui->line_number_length);

    print__actual_line(v_file, config, syntax, ui);

    if(v_file__is_cursor_y_scrolled(v_file))
    {
        for(line_i += SIZE__LINE; line_i < v_file->lines_amount; line_i++)
        {
            print__another_line(v_file, config, syntax, ui, line_i);
        }
        ui__print_line_number(v_file, config, v_file->lines_amount,
                              ui->line_number_length);

        if(v_file_last_line(v_file)->length < ui->txtarea_w)
        {
            print__line_with_tabs(v_file, config, syntax, v_file->lines_amount,
                                  0, v_file_last_line(v_file)->length);
        }
        else
        {
            print__line_with_tabs(v_file, config, syntax, v_file->lines_amount,
                                  0, (size_t) ui->txtarea_w - SIZE__LF);
        }
    }
}

void print__shrink_lines(const V_file* const v_file,
                         const Config* const config,
                         const Syntax* const syntax, const Ui* const ui)
{
    const size_t start_char_i = (-ui->punched_card_delta_x < 0) ? 0
                                : (size_t) -ui->punched_card_delta_x;
    const size_t last_line_i  = (size_t) ui->txtarea_h - SIZE__I;
    size_t       line_i       = 0;

    // Previous lines. If they are scrolled. Only a beginning is shown.
    for(; line_i < v_file_cursor_y(v_file); line_i++)
    {
        print__another_line(v_file, config, syntax, ui, line_i);
    }
    ui__print_line_number(v_file, config, v_file_cursor_y(v_file),
                          ui->line_number_length);
    print__actual_line(v_file, config, syntax, ui);

    // Next lines. If they are scrolled. Only a beginning is shown.
    for(line_i = v_file_cursor_y(v_file) + SIZE__I; line_i < last_line_i;
        line_i++)
    {
        print__another_line(v_file, config, syntax, ui, line_i);
    }
    ui__print_line_number(v_file, config, last_line_i, ui->line_number_length);

    if(v_file->lines[last_line_i].length < ui->txtarea_w)
    {
        print__line_with_tabs(v_file, config, syntax, last_line_i,
                              start_char_i,
                              v_file->lines[last_line_i].length - SIZE__LF);
    }
    else
    {
        print__line_with_tabs(v_file, config, syntax, last_line_i,
                              start_char_i,
                              (size_t) ui->txtarea_w - SIZE__LF);
    }
}

void print__scroll_lines(const V_file* const v_file,
                         const Config* const config,
                         const Syntax* const syntax, const Ui* const ui)
{
    size_t end_char_i   = v_file_actual_line(v_file)->length;
    size_t start_line_i = print__set_start_line(v_file, ui);

    // Previous lines. If they are scrolled. Only a beginning is shown.
    for(size_t line_i = start_line_i; line_i < v_file_cursor_y(v_file);
        line_i++)
    {
        print__another_line(v_file, config, syntax, ui, line_i);
    }

    // Display a last line without the linefeed to prevent a lbar breaking.
    ui__print_line_number(v_file, config, v_file_cursor_y(v_file),
                          ui->line_number_length);

    if(v_file_actual_line(v_file)->length < ui->txtarea_w)
    {
        if((v_file_actual_line(v_file)->length > 0)
           && (v_file_actual_line(v_file)->txt[
                   v_file_actual_line(v_file)->length - SIZE__NUL]
               == '\n'))
        {
            end_char_i--;
        }
        print__line_with_tabs(v_file, config, syntax,
                              v_file_cursor_y(v_file), 0, end_char_i);
    }
    // Chars won't fit in a horizontal space.
    else if((v_file_actual_line(v_file)->length - ui->txtarea_w)
            >= v_file->mirrored_cursor_x)
    {
        // Text will be scrolled. Not cursor.
        print__line_with_tabs(v_file, config, syntax,
                              v_file_cursor_y(v_file),
                              v_file_cursor_x(v_file) + SIZE__CURSOR
                              - ui->txtarea_w,
                              v_file_cursor_x(v_file));
    }
    else
    {
        // Render only left part of a line. The cursor can be scrolled.
        print__line_with_tabs(v_file, config, syntax,
                              v_file_cursor_y(v_file), 0,
                              (size_t) ui->txtarea_w - SIZE__LF);
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
    else if((v_file_cursor_y(v_file) + SIZE__I) < ui->txtarea_h)
    {
        print__shrink_lines(v_file, config, syntax, ui);
    }
    else
    {
        print__scroll_lines(v_file, config, syntax, ui);
    }
}
