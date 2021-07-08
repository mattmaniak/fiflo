#include "window.h"

term_t window__receive_terminal_size(const char axis)
{
    const unsigned sz_max = USHRT_MAX;
    const unsigned h_min  = UI__UBAR_SZ + SIZE__LINE + UI__MAX_LBAR_H;
    const unsigned w_min  = UI__GIT_LOGO_W + SIZE__SPACE + UI__GIT_BRANCH_MIN_W
                            + SIZE__SPACE + V_FILE__STATUS_MAX + SIZE__I
                            + UI__HORIZONTAL_PADDING;

    const struct winsize terminal;

    /* "TIOCGWINSZ request to the stdout descriptor. &term is required by that
        specific device (stdout)." */
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal) == -1)
    {
        fprintf(stderr, "Can't get a terminal size.\n");
        return 0;
    }

    // Terminal size check.
    if((terminal.ws_col < w_min) || (terminal.ws_row < h_min))
    {
        fprintf(stderr, "Min. terminal size: %ux%u.\n", w_min, h_min);
        return 0;
    }
    else if((terminal.ws_col > sz_max) || (terminal.ws_row > sz_max))
    {
        fprintf(stderr, "Max. terminal size: %ux%u.\n", sz_max, sz_max);
        return 0;
    }

    switch(axis)
    {
    case 'w':
        return terminal.ws_col;
    case 'h':
        return terminal.ws_row;
    }
    return 0;
}

void window__flush(void)
{
    const term_t term_h = window__receive_terminal_size('h');

    // Restore to a lowest left corner and clean a lowest line.
    ANSI__RESTORE_CURSOR_POS();
    ANSI__CLEAN_WHOLE_LINE();

    // Then from move up and clean next lines till a window ends.
    for(term_t line = UI__LBAR_SZ; line < term_h; line++)
    {
        ANSI__CURSOR_UP(1);
        ANSI__CLEAN_WHOLE_LINE();
    }
    fflush(NULL);
}

void window__fill(const V_file* const v_file, const Ui* const ui)
{
    const size_t lines_to_fill = (size_t) ui->txtarea_h - UI__LBAR_SZ;

    // Fill an empty area below a txt to adjust a position the lower bar.
    if((v_file->lines_amount + SIZE__I) < (size_t) ui->txtarea_h)
    {
        for(size_t ln = v_file->lines_amount; ln < lines_to_fill; ln++)
        {
            UI__WRAP_LINE();
        }
    }
    // Else the lower bar will by positioned by a txt.
}

void window__adjust_cursor_pos(const V_file* const v_file,
                               const Modes* const modes, const Ui* const ui)
{
    // Set by default to a filename edit.
    term_t move_right = (term_t) (UI__LEFT_PADDING + v_file->fname_len);
    term_t move_up    = (term_t) (ui->win_h - UI__LBAR_SZ);

    if(move_right >= ui->win_w)
    {
        move_right = (term_t) (ui->win_w - SIZE__CURSOR);
    }

    // Cursor is pushed right by the lower bar. Move it back.
    ANSI__CURSOR_LEFT(ui->win_w);
    ANSI__SAVE_CURSOR_POS();

    if(!modes->live_fname_edit)
    {
        if(v_file__actual_line(v_file)->len < ui->txtarea_w)
        {
            // No horizontal scrolling.
            move_right = (term_t) (ui->line_number_len
                                   + v_file__cursor_x(v_file));
        }
        else if((v_file__actual_line(v_file)->len - ui->txtarea_w)
                >= v_file->mirrored_cursor_x)
        {
            /* Last ui->txtarea_w chars are seen. Current line is scrolled,
               not cursor. */
            move_right = (term_t) (ui->win_w - SIZE__CURSOR);
        }
        else
        {
            // Text is scrolled horizontally to a start. Cursor can be moved.
            move_right = (term_t) (ui->line_number_len
                                   + v_file__cursor_x(v_file));
        }
        move_up = (v_file__cursor_y(v_file) < ui->txtarea_h)
                  ? (term_t) (ui->txtarea_h - v_file__cursor_y(v_file)
                              - SIZE__I + ui->lbar_h)
                  : ui->lbar_h;
    }
    ANSI__CURSOR_RIGHT(move_right);
    ANSI__CURSOR_UP(move_up);
}

bool window__render(const V_file* const v_file, const Config* const config,
                    const Modes* const modes, const Syntax* const syntax,
                    size_t additional_argc_i, const size_t actual_file_i)
{
    char line_number_as_str[16]; // Needed to count a len of a number.
    Ui   ui;

    sprintf(line_number_as_str, "%u",
            (int) v_file[actual_file_i].lines_amount + SIZE__I);

    ui.win_w = window__receive_terminal_size('w');
    ui.win_h = window__receive_terminal_size('h');

    if((ui.win_w == 0) || (ui.win_h == 0))
    {
        return false;
    }
    if(additional_argc_i == 0)
    {
        additional_argc_i++;
    }
    ui.expanded_lbar_h = (term_t) (UI__LBAR_SZ + additional_argc_i + SIZE__I);
    ui.lbar_h          = (modes->expanded_lbar) ? ui.expanded_lbar_h
                         : UI__LBAR_SZ;

    ui.line_number_len = (term_t) (strlen(line_number_as_str) + SIZE__SPACE
                                   + UI__LEFT_PADDING);

    ui.txtarea_w = (term_t) (ui.win_w - ui.line_number_len);
    ui.txtarea_h = (term_t) (ui.win_h - UI__UBAR_SZ - ui.lbar_h);

    ui.punched_card_delta_x = (int) (v_file[actual_file_i].mirrored_cursor_x
                                     - v_file__actual_line(v_file)->len
                                     - SIZE__I + ui.txtarea_w);

    ui.punched_card_delta_x = (ui.punched_card_delta_x > 0) ? 0
                              : ui.punched_card_delta_x;

    ui__upper_bar(&v_file[actual_file_i], config, &ui);

    print__display_text(&v_file[actual_file_i], config, syntax, &ui);
    window__fill(&v_file[actual_file_i], &ui);

    ui__lower_bar(v_file, config, modes, &ui, additional_argc_i,
                  actual_file_i);
    window__adjust_cursor_pos(&v_file[actual_file_i], modes, &ui);

    return true;
}
